#include "Graphics.hpp"

#include <glm/gtx/transform.hpp>

#ifndef VMA_IMPLEMENTATION
  #define VMA_IMPLEMENTATION
#endif

typedef MeshPushConstants PushConstants;

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
    std::string path = getenv( "PWD" );
    __assetsPath = path + "/../assets/";
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
    {
        Mesh mesh;
        obj::loadFromObj( __assetsPath + "Luke_skywalkers_landspeeder/Luke Skywalkers landspeeder.obj", mesh );
       
        vk::BufferCreateInfo bufferInfo {};
        bufferInfo.setSize( mesh.vertices().size() * sizeof(vertex0) );
        bufferInfo.setUsage( vk::BufferUsageFlagBits::eVertexBuffer );

        vma::AllocationCreateInfo allocInfo {};
        allocInfo.setUsage( vma::MemoryUsage::eCpuToGpu );

        __monkey = Mesh( mesh.vertices(), bufferInfo, allocInfo, &__allocator );
        __delQueue.pushFunction( [&](){
            auto buffer = __monkey.buffer();
            __allocator.destroyBuffer( buffer.buffer(), buffer.allocation() );
        });
    }

    /// Load all object that want to draw
    loadObject();
}

Graphics::~Graphics() 
{
    __delQueue.flush();
    std::cout << "Call Graphics Desctructor\n";
}

void Graphics::draw( vk::CommandBuffer cmd, uint32_t imageIndex, uint32_t frameNumber ) 
{
    cmd.begin( vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit} );

    // terdapat overload constructor untuk vk::ClearColorValue dan vk::ClearDepthStencilValue
    std::vector<vk::ClearValue> clearValues = { clearColorValue, clearDepthValue };

    vk::Rect2D area;
    area.setOffset( vk::Offset2D{0, 0} );
    area.setExtent( __extent );
    
    auto renderpassbegInfo = vk::RenderPassBeginInfo{};
    renderpassbegInfo.setRenderPass( __renderpass );
    renderpassbegInfo.setFramebuffer( __framebuffers[imageIndex] );
    renderpassbegInfo.setRenderArea( area );
    renderpassbegInfo.setClearValues( clearValues );

    /// Giving Command
    giveCommand( cmd, imageIndex, frameNumber, renderpassbegInfo );
}

void Graphics::giveCommand( vk::CommandBuffer& cmd, uint32_t& imageIndex, uint32_t& frameNumber, vk::RenderPassBeginInfo& rpBeginInfo ) 
{
    /// BEGIN GIVING GRAPHICS (DRAW) COMMAND
    cmd.beginRenderPass( rpBeginInfo, vk::SubpassContents::eInline );

    /// Push Constants
    PushConstants constants;
    bool isPushConstants = false;
    {
        //make a model view matrix for rendering the object
        //camera position
        glm::vec3 camPos = { 0.f,0.f,-2.f };

        glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
        //camera projection
        glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
        projection[1][1] *= -1;
        //model rotation
        glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians(frameNumber * 0.4f), glm::vec3(0, 1, 0));

        //calculate final mesh matrix
        glm::mat4 mesh_matrix = projection * view * model;

        constants.renderMatrix = std::move(mesh_matrix);
        isPushConstants = true;
    }

    /// RECORDING
    for( auto& object : __objectDraw )
    {
        auto material = object.pMaterial();
        auto mesh = object.pMesh();
        {
            if( material )
            {
                if( isPushConstants ) {
                    cmd.pushConstants( material->layout(), vk::ShaderStageFlagBits::eVertex, 0, vk::ArrayProxy<const PushConstants>{ constants } );
                }
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

void Graphics::loadObject()
{
    /// Triangle Object
    {
        ObjectDraw object{ &__triangleMesh, __material.pMaterial("triangle") };
        __objectDraw.emplace_back( object );
    }

    /// Another Object
    { 
        ObjectDraw object{ &__monkey, __material.pMaterial("triangle") };
        __objectDraw.emplace_back( object );
    }
}
