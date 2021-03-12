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
        synch.init( *_pDevice_, Synchronous::Type::FENCE_AND_SEMAPHORE, vk::FenceCreateFlagBits::eSignaled );
        synch.create();
    }


    _hasBeenInit_ = true;
    _hasBeenCreated_ = true;
}

void Engine::loop() 
{
    while( !_window_.shouldClose() )
    {
        _window_.poolEvents();
    }
}

void Engine::destroy() 
{
    assert( _hasBeenCreated_ );

    for( auto& synch : _syncrhonouses_ )
        synch.destroy();

    for( auto& cmd : _commands_ )
        cmd.destroy();

    _vulkanbase_.destroy();     // must be the 2nd LAST that 'll destroy
    _window_.destroy();         // must be the 1st LAST that 'll destroy

    _hasBeenCreated_ = false;
}
