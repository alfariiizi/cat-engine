#include "Graphics.hpp"


Graphics::~Graphics() 
{
    destroy();
}

void Graphics::init( vk::PhysicalDevice physicalDevice, const vk::Device& device, const Renderpass::State& renderpass, uint32_t width, uint32_t height, uint32_t queueIndex, vk::Queue graphicsQueue, vk::Queue presentQueue ) 
{
    _physicalDevice_ = physicalDevice;
    _pDevice_ = &device;
    _renderpass_ = renderpass;
    _queue_._graphics = graphicsQueue;
    _queue_._present = presentQueue;
    _width_ = width;
    _height_ = height;

    material.init( _physicalDevice_, *_pDevice_, _renderpass_.getRenderpass(), queueIndex, _queue_._graphics, _width_, _height_ );

    vertBuffer = { *_pDevice_, _physicalDevice_.getMemoryProperties(), vertices };


    _hasBeenInit_ = true;
    _hasBeenCreated_ = true;
}

void Graphics::destroy() 
{
    assert( _hasBeenInit_ );

    if( _hasBeenCreated_ )
    {
        material.destroy();

        vertBuffer.~HostVertexBuffer();

        _hasBeenCreated_ = false;
    }
}

void Graphics::draw(vk::CommandBuffer cmd, uint32_t imageIndex ) 
{
    cmd.begin( vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit} );

    // terdapat overload constructor untuk vk::ClearColorValue dan vk::ClearDepthStencilValue
    std::vector<vk::ClearValue> clearValues = { clearColorValue, clearDepthValue };

    vk::Rect2D area;
    area.setOffset( vk::Offset2D{0, 0} );
    area.setExtent( vk::Extent2D{ _width_, _height_ } );
    auto renderpassBeginInfo = vk::RenderPassBeginInfo{
        _renderpass_.getRenderpass(),
        _renderpass_.getFramebuffer()[imageIndex],
        area,
        clearValues
    };

    /// Giving Command
    giveCommand( cmd, imageIndex, renderpassBeginInfo );
}

void Graphics::giveCommand( vk::CommandBuffer cmd, uint32_t imageIndex, vk::RenderPassBeginInfo& rpBeginInfo ) 
{
    /// BEGIN GIVING GRAPHICS (DRAW) COMMAND
    cmd.beginRenderPass( rpBeginInfo, vk::SubpassContents::eInline );

    /// RECORDING
    auto it_material = material.getMaterial( "basicTextured" );
    {

        cmd.bindPipeline( vk::PipelineBindPoint::eGraphics, it_material->_pipeline );
        cmd.bindVertexBuffers( 0, vertBuffer.buffer(), vk::DeviceSize{0} );
        cmd.bindDescriptorSets( vk::PipelineBindPoint::eGraphics, it_material->_layout, 0, it_material->_set, nullptr );
        cmd.draw( vertices.size(), 1, 0, 0 );
    }

    /// END GIVING GRAPHICS (DRAW) COMMAND
    cmd.endRenderPass();

    // END GIVING ALL KIND OF COMMANDS
    cmd.end();
}
