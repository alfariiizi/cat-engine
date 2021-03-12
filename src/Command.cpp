#include "Command.hpp"

void Command::init( const vk::Device& device, uint32_t graphicsQueueFamilyIndices, uint32_t bufferSize, vk::CommandBufferLevel bufferLevel ) 
{
    _pDevice_ = &device;
    _graphicsQueueFamilyIndices_ = graphicsQueueFamilyIndices;
    _bufferSize_ = bufferSize;
    _bufferLevel_ = bufferLevel;

    _hasBeenInit_ = true;
}

void Command::create() 
{
    assert( _hasBeenInit_ );

    /**
     * @brief Command Pool
     */
    vk::CommandPoolCreateInfo cmdPoolInfo {};
    cmdPoolInfo.setFlags( vk::CommandPoolCreateFlagBits::eResetCommandBuffer );
    cmdPoolInfo.setQueueFamilyIndex( _graphicsQueueFamilyIndices_ );

    _pool_ = _pDevice_->createCommandPool( cmdPoolInfo );


    /**
     * @brief Command Buffers
     */
    vk::CommandBufferAllocateInfo cmdBufferAllocInfo {};
    cmdBufferAllocInfo.setCommandPool( _pool_ );
    cmdBufferAllocInfo.setLevel( _bufferLevel_ );
    cmdBufferAllocInfo.setCommandBufferCount( _bufferSize_ );

    _buffers_ = _pDevice_->allocateCommandBuffers( cmdBufferAllocInfo );


    _hasBeenCreated_ = true;
}

void Command::destroy() 
{
    assert( _hasBeenCreated_ );

    _pDevice_->destroyCommandPool( _pool_ );
}

const vk::CommandPool& Command::getPool() 
{
    assert( _hasBeenCreated_ );
    return _pool_;
}

const std::vector<vk::CommandBuffer>& Command::getBuffers() 
{
    assert( _hasBeenCreated_ );
    return _buffers_;
}