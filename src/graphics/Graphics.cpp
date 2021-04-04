#include "Graphics.hpp"
#include "ShaderStruct.hpp"

Graphics::Graphics(vk::PhysicalDevice physicalDevice,
                   vk::Device device,
                   vk::RenderPass rp,
                   std::vector<vk::Framebuffer> fbs,
                   const vk::Extent2D &extent,
                   uint32_t queueIndex,
                   vk::Queue queue)
    :
    __physicalDevice( physicalDevice ),
    __device( device ),
    __renderpass( rp ),
    __framebuffers( fbs ),
    __extent( extent ),
    __queueFamilyIndex( queueIndex ),
    __queue( queue ),
    __material( physicalDevice, device, rp, queueIndex, queue, extent.width, extent.height )
{
    // vertBuffer = vku::VertexBuffer{ device, physicalDevice.getMemoryProperties(), sizeof(Texture_Vertex) * vertices.size() };
    vertBuffer = vku::HostVertexBuffer{ __device, __physicalDevice.getMemoryProperties(), vertices };
}

Graphics::~Graphics() 
{
    __delQueue.flush();
}


void Graphics::draw(vk::CommandBuffer cmd, uint32_t imageIndex ) 
{
    cmd.begin( vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit} );

    // terdapat overload constructor untuk vk::ClearColorValue dan vk::ClearDepthStencilValue
    std::vector<vk::ClearValue> clearValues = { clearColorValue, clearDepthValue };

    vk::Rect2D area;
    area.setOffset( vk::Offset2D{0, 0} );
    area.setExtent( __extent );
    // auto renderpassBeginInfo = vk::RenderPassBeginInfo{
    //     __renderpass,
    //     __framebuffers[imageIndex],
    //     area,
    //     clearValues
    // };
    
    auto renderpassbegInfo = vk::RenderPassBeginInfo{};
    renderpassbegInfo.setRenderPass( __renderpass );
    renderpassbegInfo.setFramebuffer( __framebuffers[imageIndex] );
    renderpassbegInfo.setRenderArea( area );
    renderpassbegInfo.setClearValues( clearValues );

    /// Giving Command
    giveCommand( cmd, imageIndex, renderpassbegInfo );
}

void Graphics::giveCommand( vk::CommandBuffer cmd, uint32_t imageIndex, vk::RenderPassBeginInfo& rpBeginInfo ) 
{
    /// BEGIN GIVING GRAPHICS (DRAW) COMMAND
    cmd.beginRenderPass( rpBeginInfo, vk::SubpassContents::eInline );

    /// RECORDING
    auto pipeline = __material.getPipeline("triangle");
    {
        cmd.bindPipeline( vk::PipelineBindPoint::eGraphics, pipeline->_pipeline );
        cmd.bindVertexBuffers( 0, vertBuffer.buffer(), vk::DeviceSize{0} );
        // cmd.bindDescriptorSets( vk::PipelineBindPoint::eGraphics, it_material->_layout, 0, it_material->_set, nullptr );
        cmd.draw( vertices.size(), 1, 0, 0 );
    }

    /// END GIVING GRAPHICS (DRAW) COMMAND
    cmd.endRenderPass();

    // END GIVING ALL KIND OF COMMANDS
    cmd.end();
}