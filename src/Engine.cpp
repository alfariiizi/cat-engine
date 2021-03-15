#include "Engine.hpp"


Engine::Engine() 
{
    init();
}

Engine::~Engine() 
{
    destroy();
}

void Engine::init() 
{
    _window_.init();
    _window_.create();

    _vulkanbase_.init( _window_ );  // this is strange, but at this time VulkanBase really don't have create function

    _pDevice_ = &_vulkanbase_.getDevice();
    _pAllocator_ = &_vulkanbase_.getAllocator();

    for( auto& cmd : _commands_ )
    {
        cmd.init( *_pDevice_, _vulkanbase_.getSwapchain().getGraphicsQueueFamilyIndices(), 1, vk::CommandBufferLevel::ePrimary );
        cmd.create();
    }

    for( auto& synch : _syncrhonouses_ )
    {
        synch._renderFence = _pDevice_->createFence( {vk::FenceCreateFlagBits::eSignaled} );
        synch._renderSemaphore = _pDevice_->createSemaphore( {} );
        synch._presentSemaphore = _pDevice_->createSemaphore( {} );
    }

    _graphics_.init( _vulkanbase_.getPhysicalDevice(), *_pDevice_, _vulkanbase_.getRenderpass(), _window_.ScreenWidth, _window_.ScreenHeight, _vulkanbase_.getGraphicsQueue(), _vulkanbase_.getPresentQueue() );


    _hasBeenInit_ = true;
    _hasBeenCreated_ = true;
}

void Engine::draw() 
{
    auto cmd = _commands_[_frameInUse_].getBuffers().front();
    auto presentSemaphore = _syncrhonouses_[_frameInUse_]._presentSemaphore;
    auto renderSemaphore = _syncrhonouses_[_frameInUse_]._renderSemaphore;
    auto renderFence = _syncrhonouses_[_frameInUse_]._renderFence;

    vk::Result result = _pDevice_->waitForFences( renderFence, VK_TRUE, UINT64_MAX );
    _pDevice_->resetFences( renderFence );

    uint32_t imageIndex = _pDevice_->acquireNextImageKHR( _vulkanbase_.getSwapchain().getSwapchain(), UINT64_MAX, presentSemaphore, nullptr ).value;
    _graphics_.draw( cmd, imageIndex );

    /**
     * @brief Submit Info ( it could be graphics queue, compute queue, or maybe transfer queue )
     */
    vk::SubmitInfo submitInfo {};
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    submitInfo.setWaitSemaphores( presentSemaphore );
    submitInfo.setWaitDstStageMask( waitStage );
    submitInfo.setCommandBuffers( cmd );
    submitInfo.setSignalSemaphores( renderSemaphore );

    _vulkanbase_.getGraphicsQueue().submit( submitInfo, renderFence );


    /**
     * @brief Present Info ( it's just the present queue )
     */
    vk::PresentInfoKHR presentInfo {};
    presentInfo.setWaitSemaphores( renderSemaphore );

    std::vector<vk::SwapchainKHR> swapchains = { _vulkanbase_.getSwapchain().getSwapchain() };
    presentInfo.setSwapchains( swapchains );
    presentInfo.setImageIndices( imageIndex );

    result = _vulkanbase_.getPresentQueue().presentKHR( presentInfo );
    if( result != vk::Result::eSuccess )
        throw std::runtime_error( "Failed to presenting (_presentQueue)" );
}

void Engine::loop() 
{
    while( !_window_.shouldClose() )
    {
        _window_.poolEvents();

        draw();

        ++_frameNumber_;
        ++_frameInUse_;
        _frameInUse_ %= MAX_FRAME;
    }
}

void Engine::destroy() 
{
    if( _hasBeenCreated_ )
    {
        _pDevice_->waitIdle();

        _graphics_.destroy();

        for( auto& synch : _syncrhonouses_ )
        {
            _pDevice_->destroySemaphore( synch._renderSemaphore );
            _pDevice_->destroySemaphore( synch._presentSemaphore );
            _pDevice_->destroyFence( synch._renderFence );
        }

        for( auto& cmd : _commands_ )
            cmd.destroy();

        _vulkanbase_.destroy();     // must be the 2nd LAST that 'll destroy
        _window_.destroy();         // must be the 1st LAST that 'll destroy

        _hasBeenCreated_ = false;
    }
}
