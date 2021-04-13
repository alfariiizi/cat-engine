#include "Graphics.hpp"
#include "ShaderStruct.hpp"

#ifndef VMA_IMPLEMENTATION
  #define VMA_IMPLEMENTATION
#endif

Graphics::Graphics(vk::PhysicalDevice physicalDevice,
                   vk::Device device,
                   vma::Allocator allocator,
                   vk::RenderPass rp,
                   std::vector<vk::Framebuffer> fbs,
                   const vk::Extent2D &extent,
                   uint32_t queueIndex,
                   vk::Queue queue)
    :
    __physicalDevice( physicalDevice ),
    __device( device ),
    __allocator( allocator ),
    __renderpass( rp ),
    __framebuffers( fbs ),
    __extent( extent ),
    __queueFamilyIndex( queueIndex ),
    __queue( queue ),
    __material( physicalDevice, device, rp, queueIndex, queue, extent.width, extent.height )
{
    {
        std::vector<Vertex::SimpleVertex> vertices( 3 );
        vertices[0].position = { 0.5f, -0.5f, 0.0f };
        vertices[1].position = { -0.5f, -0.5f, 0.0f };
        vertices[2].position = { 0.0f, 0.5f, 0.0f };
        vertices[0].color = { 1.0f, 0.0f, 0.0f };
        vertices[1].color = { 0.0f, 1.0f, 0.0f };
        vertices[2].color = { 0.0f, 0.0f, 1.0f };

        vk::BufferCreateInfo bufferInfo {};
        bufferInfo.setSize( vertices.size() * sizeof(Vertex::SimpleVertex) );
        bufferInfo.setUsage( vk::BufferUsageFlagBits::eVertexBuffer );
        // bufferInfo.setQueueFamilyIndices( __queueFamilyIndex );
        // bufferInfo.setSharingMode( vk::SharingMode::eExclusive );

        vma::AllocationCreateInfo allocInfo {};
        allocInfo.setUsage( vma::MemoryUsage::eCpuToGpu );

        __triangleMesh = Mesh( std::move(vertices), bufferInfo, allocInfo, &__allocator );
        __delQueue.pushFunction([&](){
            auto buffer = __triangleMesh.buffer();
            __allocator.destroyBuffer( buffer.buffer(), buffer.allocation() );
        });
    }

    loadObject();
}

Graphics::~Graphics() 
{
    __delQueue.flush();
    std::cout << "Call Graphics Desctructor\n";
}

void Graphics::loadObject()
{
    ObjectDraw object{ &__triangleMesh, __material.pMaterial("triangle") };
    __objectDraw.emplace_back( object );
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
    for( auto& object : __objectDraw )
    {
        auto material = object.pMaterial();
        auto mesh = object.pMesh();
        {
            if( material )
            {
                cmd.bindPipeline( vk::PipelineBindPoint::eGraphics, material->pipeline() );
            }
            if( mesh )
            {
                cmd.bindVertexBuffers( 0, mesh->buffer().buffer(), vk::DeviceSize{0} );
                cmd.draw( mesh->vertices().size(), 1, 0, 0 );
            }
            // cmd.bindDescriptorSets( vk::PipelineBindPoint::eGraphics, it_material->_layout, 0, it_material->_set, nullptr );
        }
    }

    /// END GIVING GRAPHICS (DRAW) COMMAND
    cmd.endRenderPass();

    // END GIVING ALL KIND OF COMMANDS
    cmd.end();
}