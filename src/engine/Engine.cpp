#include "Engine.hpp"
#include "vk_utils.hpp"


Engine::Engine() 
    :
    __vulkanbase( __window ),
    __device( __vulkanbase._pDevice.get() ),
    __graphics( __vulkanbase._physicalDevice, __vulkanbase._pDevice.get(), __vulkanbase._pRenderpass.get(), __vulkanbase.getFramebuffers(), {Window::ScreenWidth, Window::ScreenHeight}, utils::FindQueueFamilyIndices( __vulkanbase._physicalDevice, __vulkanbase._surface ).graphicsFamily.value(), __vulkanbase._graphicsQueue )
{
    // init();

    for( int i = 0; i < MAX_FRAME; ++i )
    {
        { /// Command
            auto poolInfo = vk::CommandPoolCreateInfo { vk::CommandPoolCreateFlagBits::eResetCommandBuffer, utils::FindQueueFamilyIndices( __vulkanbase._physicalDevice, __vulkanbase._surface ).graphicsFamily.value() };
            __commands[i]._pCmdPool = __device.createCommandPoolUnique( poolInfo );
            auto buffInfo = vk::CommandBufferAllocateInfo { __commands[i]._pCmdPool.get(), vk::CommandBufferLevel::ePrimary, 1 };
            __commands[i]._pCmdBuffer = std::move(__device.allocateCommandBuffersUnique( buffInfo ).front());
        }
        { /// Synch
            __syncrhonouses[i]._renderFence = __device.createFenceUnique( {vk::FenceCreateFlagBits::eSignaled} );
            __syncrhonouses[i]._renderSemaphore = __device.createSemaphoreUnique( {} );
            __syncrhonouses[i]._presentSemaphore = __device.createSemaphoreUnique( {} );
        }
    }


}

Engine::~Engine() 
{
    __delQueue.flush();
}

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
    auto cmd = __commands[__frameInUse]._pCmdBuffer.get();
    auto presentSemaphore = __syncrhonouses[__frameInUse]._presentSemaphore.get();
    auto renderSemaphore = __syncrhonouses[__frameInUse]._renderSemaphore.get();
    auto renderFence = __syncrhonouses[__frameInUse]._renderFence.get();

    vk::Result result = __vulkanbase._pDevice->waitForFences( renderFence, VK_TRUE, UINT64_MAX );
    __vulkanbase._pDevice->resetFences( renderFence );

    uint32_t imageIndex = __vulkanbase._pDevice->acquireNextImageKHR( __vulkanbase._pSwapchain.get(), UINT64_MAX, presentSemaphore, nullptr ).value;
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

    __vulkanbase._graphicsQueue.submit( submitInfo, renderFence );


    /**
     * @brief Present Info ( it's just the present queue )
     */
    vk::PresentInfoKHR presentInfo {};
    presentInfo.setWaitSemaphores( renderSemaphore );

    std::vector<vk::SwapchainKHR> swapchains = { __vulkanbase._pSwapchain.get() };
    presentInfo.setSwapchains( swapchains );
    presentInfo.setImageIndices( imageIndex );

    result = __vulkanbase._presentQueue.presentKHR( presentInfo );
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
}

void Engine::destroy() 
{
    // if( _hasBeenCreated_ )
    // {
    //     __vulkanbase._pDevice->waitIdle();

    //     __graphics.destroy();

    //     for( auto& synch : __syncrhonouses )
    //     {
    //         __vulkanbase._pDevice->destroySemaphore( synch._renderSemaphore );
    //         __vulkanbase._pDevice->destroySemaphore( synch._presentSemaphore );
    //         __vulkanbase._pDevice->destroyFence( synch._renderFence );
    //     }

    //     for( auto& cmd : __commands )
    //         cmd.destroy();

    //     __vulkanbase.destroy();     // must be the 2nd LAST that 'll destroy
    //     __window.destroy();         // must be the 1st LAST that 'll destroy

    //     _hasBeenCreated_ = false;
    // }
}
