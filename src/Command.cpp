#include "Command.hpp"


Command::~Command() 
{
    _delQueue_.flush();
}

void Command::init( const vk::Device& device, uint32_t graphicsQueueFamilyIndices, uint32_t bufferSize, vk::CommandBufferLevel bufferLevel ) 
{
    _device_ = device;
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

    _uPool_ = _device_.createCommandPoolUnique( cmdPoolInfo );


    /**
     * @brief Command Buffers
     */
    vk::CommandBufferAllocateInfo cmdBufferAllocInfo {};
    cmdBufferAllocInfo.setCommandPool( _uPool_.get() );
    cmdBufferAllocInfo.setLevel( _bufferLevel_ );
    cmdBufferAllocInfo.setCommandBufferCount( _bufferSize_ );

    _uBuffers_ = _device_.allocateCommandBuffersUnique( cmdBufferAllocInfo );


    _hasBeenCreated_ = true;
}

void Command::destroy() 
{
    // assert( _hasBeenCreated_ );

    // _device_.destroyCommandPool( _uPool_ );
}

const vk::CommandPool& Command::getPool() 
{
    assert( _hasBeenCreated_ );
    return _uPool_.get();
}

const std::vector<vk::CommandBuffer>& Command::getBuffers() 
{
    assert( _hasBeenCreated_ );
    std::vector<vk::CommandBuffer> buffers;
    buffers.reserve( _uBuffers_.size() );

    for( auto& b : _uBuffers_ )
        buffers.emplace_back( b.get() );

    return buffers;
}
