#include "Engine.hpp"
#include "vk_utils.hpp"


Engine::Engine() 
    :
    __vulkanbase( __window ),
    __device( __vulkanbase.device() ),
    __graphics( __vulkanbase.physicalDevice(), __vulkanbase.device(), __vulkanbase.allocator(), __vulkanbase.renderpass(), __vulkanbase.framebuffers(), {Window::ScreenWidth, Window::ScreenHeight}, utils::FindQueueFamilyIndices( __vulkanbase.physicalDevice(), __vulkanbase.surface() ).graphicsFamily.value(), __vulkanbase.graphicsPresentQueue().graphicsQueue() )
{
    for( int i = 0; i < MAX_FRAME; ++i )
    {
        { /// Command
            auto poolInfo = vk::CommandPoolCreateInfo { vk::CommandPoolCreateFlagBits::eResetCommandBuffer, utils::FindQueueFamilyIndices( __vulkanbase.physicalDevice(), __vulkanbase.surface() ).graphicsFamily.value() };
            __commands[i]._cmdPool = __device.createCommandPool( poolInfo );
            auto buffInfo = vk::CommandBufferAllocateInfo { __commands[i]._cmdPool, vk::CommandBufferLevel::ePrimary, 1 };
            __commands[i]._cmdBuffer = __device.allocateCommandBuffers( buffInfo ).front();
            __delQueue.pushFunction([d=__device, cmd=__commands[i]._cmdPool](){
                d.destroyCommandPool( cmd );
            });
        }
        { /// Synch
            __syncrhonouses[i]._renderFence = __device.createFence( {vk::FenceCreateFlagBits::eSignaled} );
            __syncrhonouses[i]._renderSemaphore = __device.createSemaphore( {} );
            __syncrhonouses[i]._presentSemaphore = __device.createSemaphore( {} );
            __delQueue.pushFunction([d=__device, syc=__syncrhonouses[i]](){
                d.destroySemaphore( syc._presentSemaphore );
                d.destroySemaphore( syc._renderSemaphore );
                d.destroyFence( syc._renderFence );
            });
        }
    }
    // __delQueue.pushFunction([&](){
    //     for( int i = 0; i < MAX_FRAME; ++i )
    //     {
    //         __sync
    //         __device.destroySemaphore( )
    //     }
    // });


}

// Engine::~Engine() 
// {
//     __delQueue.flush();
// }

void Engine::init() 
{
    // __window.init();
    // __window.create();

    // __vulkanbase.init( __window );  // this is strange, but at this time VulkanBase really don't have create function

    // __vulkanbase._pDevice = &__vulkanbase.getDevice();
    // _pAllocator_ = &__vulkanbase.getAllocator();

    // for( auto& cmd : __commands )
    // {
    //     cmd.init( *__vulkanbase._pDevice, __vulkanbase.getSwapchain().getGraphicsQueueFamilyIndices(), 1, vk::CommandBufferLevel::ePrimary );
    //     cmd.create();
    // }

    // for( auto& synch : __syncrhonouses )
    // {
    //     synch._renderFence = __vulkanbase._pDevice->createFence( {vk::FenceCreateFlagBits::eSignaled} );
    //     synch._renderSemaphore = __vulkanbase._pDevice->createSemaphore( {} );
    //     synch._presentSemaphore = __vulkanbase._pDevice->createSemaphore( {} );
    // }

    // __graphics.init( __vulkanbase.getPhysicalDevice(), 
    //                     *__vulkanbase._pDevice, 
    //                     __vulkanbase.getRenderpass(), 
    //                     __window.ScreenWidth, __window.ScreenHeight, 
    //                     utils::FindQueueFamilyIndices( __vulkanbase.getPhysicalDevice(), __vulkanbase.getSurface()).graphicsAndPresentFamilyIndex().front(), 
    //                     __vulkanbase.getGraphicsQueue(), __vulkanbase.getPresentQueue() );
}

void Engine::draw() 
{
    auto cmd = __commands[__frameInUse]._cmdBuffer;
    auto presentSemaphore = __syncrhonouses[__frameInUse]._presentSemaphore;
    auto renderSemaphore = __syncrhonouses[__frameInUse]._renderSemaphore;
    auto renderFence = __syncrhonouses[__frameInUse]._renderFence;
    auto queue = __vulkanbase.graphicsPresentQueue();

    vk::Result result = __device.waitForFences( renderFence, VK_TRUE, UINT64_MAX );
    __device.resetFences( renderFence );

    uint32_t imageIndex = __device.acquireNextImageKHR( __vulkanbase.swapchain(), UINT64_MAX, presentSemaphore, nullptr ).value;
    __graphics.draw( cmd, imageIndex );

    /**
     * @brief Submit Info ( it could be graphics queue, compute queue, or maybe transfer queue )
     */
    vk::SubmitInfo submitInfo {};
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    submitInfo.setWaitSemaphores( presentSemaphore );
    submitInfo.setWaitDstStageMask( waitStage );
    submitInfo.setCommandBuffers( cmd );
    submitInfo.setSignalSemaphores( renderSemaphore );

    queue.graphicsQueue().submit( submitInfo, renderFence );


    /**
     * @brief Present Info ( it's just the present queue )
     */
    vk::PresentInfoKHR presentInfo {};
    presentInfo.setWaitSemaphores( renderSemaphore );

    std::vector<vk::SwapchainKHR> swapchains = { __vulkanbase.swapchain() };
    presentInfo.setSwapchains( swapchains );
    presentInfo.setImageIndices( imageIndex );

    result = queue.presentQueue().presentKHR( presentInfo );
    if( result != vk::Result::eSuccess )
        throw std::runtime_error( "Failed to presenting (_presentQueue)" );
}

void Engine::loop() 
{
    while( !__window.shouldClose() )
    {
        __window.poolEvents();

        draw();

        ++__frameNumber;
        ++__frameInUse;
        __frameInUse %= MAX_FRAME;
    }

    __device.waitIdle();
    destroy();
}

void Engine::destroy() 
{
    __delQueue.flush();
}
