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

    /// Create Command Pool for One time submit cmdBuffer and Descriptor Pool
    {
        // Command Pool
        auto poolInfo = vk::CommandPoolCreateInfo {
            vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            queueIndex
        };
        __cmdPool = __device.createCommandPool( poolInfo );
        __delQueue.pushFunction([&](){
            __device.destroyCommandPool( __cmdPool );
        });

        // Descriptor Pool
        std::vector<vk::DescriptorPoolSize> sizes = {
            { vk::DescriptorType::eCombinedImageSampler, 10U },
            { vk::DescriptorType::eSampledImage, 10U },
            { vk::DescriptorType::eSampler, 10U },
            { vk::DescriptorType::eStorageBuffer, 10U },
            { vk::DescriptorType::eStorageBufferDynamic, 10U },
            { vk::DescriptorType::eStorageImage, 10U },
            { vk::DescriptorType::eStorageTexelBuffer, 10U },
            { vk::DescriptorType::eUniformBuffer, 10U },
            { vk::DescriptorType::eUniformBufferDynamic, 10U },
            { vk::DescriptorType::eUniformTexelBuffer, 10U },
        };
        vk::DescriptorPoolCreateInfo setPoolInfo {};
        setPoolInfo.setPoolSizes( sizes );
        setPoolInfo.setMaxSets( 10 );
        __setPool = __device.createDescriptorPool( setPoolInfo );
        __delQueue.pushFunction([&](){
            __device.destroyDescriptorPool( __setPool );
        });
    }

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
        allocInfo.setUsage( vma::MemoryUsage::eGpuOnly );

        __triangleMesh = Mesh( __device, __cmdPool, __queue, std::move(vertices), bufferInfo, allocInfo, &__allocator );
        __delQueue.pushFunction([&](){
            auto buffer = __triangleMesh.buffer();
            __allocator.destroyBuffer( buffer.buffer(), buffer.allocation() );
        });
    }

    /// Create All the Meshes ( actually, it's still just monkey :) )
    {
        Mesh mesh;
        obj::loadFromObj( __assetsPath + "monkey_smooth.obj", mesh );

        vk::BufferCreateInfo bufferInfo {};
        bufferInfo.setSize( mesh.vertices().size() * sizeof(vertex0) );
        bufferInfo.setUsage( vk::BufferUsageFlagBits::eVertexBuffer );

        vma::AllocationCreateInfo allocInfo {};
        allocInfo.setUsage( vma::MemoryUsage::eGpuOnly );

        __monkey = Mesh( __device, __cmdPool, __queue, std::move(mesh.vertices()), bufferInfo, allocInfo, &__allocator );
        __delQueue.pushFunction( [&](){
            auto buffer = __monkey.buffer();
            __allocator.destroyBuffer( buffer.buffer(), buffer.allocation() );
        });
    }

    /// Creating all descriptor sets that in need
    createDescriptorSet();

    /// Load all object that want to draw
    loadObject();
}

Graphics::~Graphics() 
{
    __device.waitIdle();
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


    /// RECORDING
    for( auto& object : __objectDraw )
    {
        auto material = object.pMaterial();
        auto mesh = object.pMesh();
        auto transform = object.transformMatrix();
        // auto ubo = __ubos[imageIndex];
        auto camBuff = __ubos._buffers[0];
        auto sceneBuff = __ubos._buffers[1];

        /// Push Constants
        PushConstants constants;
        bo::CameraData camData;
        bo::SceneData sceneData;
        bool isPushConstants = false;
        {
            uint32_t frameIndex = frameNumber % MAX_FRAME;

            // Camera
            {
                // make a model view matrix for rendering the object
                // camera view
                glm::vec3 camPos = { 0.f,-6.f,-10.f };

                glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
                // camera projection
                glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
                projection[1][1] *= -1;

                // fill a GPU camera data struct
                camData.proj = projection;
                camData.view = view;
                camData.viewProj = projection * view;

                char* camPtr;
                __allocator.mapMemory( camBuff.allocation(), (void**)&camPtr );
                camPtr += padUniformBufferSize( sizeof(bo::CameraData) ) * frameIndex;
                memcpy( camPtr, &camData, sizeof( bo::CameraData ) );
                __allocator.unmapMemory( camBuff.allocation() );
            }
            // Scene
            {
                float framed = frameNumber / 120.0f;
                sceneData.ambientColor = { sin(framed), 0, cos(framed), 1 };
                char* scenePtr;
                __allocator.mapMemory( sceneBuff.allocation(), (void**)&scenePtr );
                scenePtr += padUniformBufferSize( sizeof(bo::SceneData) ) * frameIndex;
                memcpy( scenePtr, &sceneData, sizeof( bo::SceneData ) );
                __allocator.unmapMemory( sceneBuff.allocation() );
            }


            // //camera position
            // glm::vec3 camPos = { 0.f,0.f,-2.f };
            // //camera view
            // glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
            // //camera projection
            // glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
            // projection[1][1] *= -1;

            // //model rotation
            // glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians(frameNumber * 0.4f), glm::vec3(0, 1, 0));
            // glm::mat4 model = transform;

            // //calculate final mesh matrix
            // glm::mat4 mesh_matrix = projection * view * model;

            // constants.renderMatrix = std::move(mesh_matrix);
            constants.renderMatrix = object.transformMatrix();
            isPushConstants = true;
        }

        {
            if( material )
            {
                // Descriptor bind
                // cmd.bindDescriptorSets( vk::PipelineBindPoint::eGraphics, material->layout(), 0, __ubos[imageIndex]._set, nullptr );
                cmd.bindDescriptorSets( vk::PipelineBindPoint::eGraphics, material->layout(), 0, __ubos._set, nullptr );

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
        }
    }

    /// END GIVING GRAPHICS (DRAW) COMMAND
    cmd.endRenderPass();

    // END GIVING ALL KIND OF COMMANDS
    cmd.end();
}

void Graphics::loadObject()
{
    /// Monkey Object
    { 
        ObjectDraw object{ &__monkey, __material.pMaterial("triangle"), glm::mat4( 1 ) };
        __objectDraw.emplace_back( object );
    }

    /// Triangle Object
    {
        for( int x = -20; x <= 20; ++x ) {
            for( int y = -20; y <= 20; ++y ) {
                glm::mat4 translation = glm::translate( glm::mat4{ 1.0f }, glm::vec3{ x, 0, y } );
                glm::mat4 scale = glm::scale( glm::mat4{ 1.0f }, glm::vec3{ 0.2f, 0.2f, 0.2f } );
                ObjectDraw object{ &__triangleMesh, __material.pMaterial("triangle"), translation * scale };

                __objectDraw.emplace_back( object );
            }
        }
    }

}

void Graphics::createDescriptorSet() 
{
    /// UBO for every frame
    {
        __ubos._buffers.resize( 2 );    // Camera Data and Scene Data

        // uint32_t totalSize = MAX_FRAME * ( padUniformBufferSize( sizeof( bo::CameraData ) ) + padUniformBufferSize( sizeof(bo::SceneData) ) ) ;
        uint32_t cameraDataSize = MAX_FRAME * padUniformBufferSize( sizeof(bo::CameraData) );
        uint32_t sceneDataSize = MAX_FRAME * padUniformBufferSize( sizeof(bo::SceneData) );

        auto cameraDataDescType = vk::DescriptorType::eUniformBuffer;
        auto sceneDataDescType = vk::DescriptorType::eUniformBuffer;

        auto bufferUsage = vk::BufferUsageFlagBits::eUniformBuffer;

        // buffer
        {
            // Camera Data
            {
                vk::BufferCreateInfo bufferInfo {};
                bufferInfo.setSize( cameraDataSize );
                bufferInfo.setUsage( bufferUsage );
                vma::AllocationCreateInfo allocInfo {};
                allocInfo.setUsage( vma::MemoryUsage::eCpuToGpu );
                // for( auto& b : __ubos ) {
                //     auto destroyer = b._buffer.createBuffer( __allocator, bufferInfo, allocInfo );
                //     __delQueue.pushFunction( destroyer );
                // }
                auto destroyer = __ubos._buffers[0].createBuffer( __allocator, bufferInfo, allocInfo );
                __delQueue.pushFunction( destroyer );
            }

            // Scene Data
            {
                vk::BufferCreateInfo bufferInfo {};
                bufferInfo.setSize( sceneDataSize );
                bufferInfo.setUsage( bufferUsage );
                vma::AllocationCreateInfo allocInfo {};
                allocInfo.setUsage( vma::MemoryUsage::eCpuToGpu );
                auto destroyer = __ubos._buffers[1].createBuffer( __allocator, bufferInfo, allocInfo );
                __delQueue.pushFunction( destroyer );
            }
        }

        // descriptor set
        {
            auto setLayout = __material.pSetLayout("ubo");
            assert( setLayout );
            // __ubos._sets.resize( 2 );

            vk::DescriptorSetAllocateInfo allocInfo {};
            allocInfo.setDescriptorPool( __setPool );
            allocInfo.setDescriptorSetCount( 1 );
            allocInfo.setSetLayouts( *setLayout );
            __ubos._set = __device.allocateDescriptorSets( allocInfo ).front();

            for( uint32_t i = 0; i < MAX_FRAME; ++i )
            {

                // to make sure that the creting object are not desctructed before the object has updated
                uint32_t totalBindings = 2;
                std::vector<vk::DescriptorBufferInfo> bufferInfos( totalBindings );
                std::vector<vk::WriteDescriptorSet> setWrites( totalBindings );
                {
                    uint32_t bindingNum = 0;

                    vk::DescriptorBufferInfo bufferInfo {};
                    bufferInfo.setBuffer( __ubos._buffers[0].buffer() );
                    bufferInfo.setOffset( padUniformBufferSize(sizeof(bo::CameraData)) * i );
                    bufferInfo.setRange( sizeof(bo::CameraData) );
                    bufferInfos[bindingNum] = std::move( bufferInfo );

                    vk::WriteDescriptorSet setWrite {};
                    setWrite.setDescriptorCount( 1 );
                    setWrite.setDescriptorType( cameraDataDescType );
                    setWrite.setDstBinding( 0 );
                    setWrite.setDstSet( __ubos._set );
                    setWrite.setBufferInfo( bufferInfos[bindingNum] );
                    setWrites[bindingNum] = std::move( setWrite );
                }
                {
                    uint32_t bindingNum = 1;

                    vk::DescriptorBufferInfo bufferInfo {};
                    bufferInfo.setOffset( padUniformBufferSize( sizeof(bo::SceneData)) * i );
                    bufferInfo.setBuffer( __ubos._buffers[1].buffer() );
                    bufferInfo.setRange( sizeof(bo::SceneData) );
                    bufferInfos[bindingNum] = std::move( bufferInfo );

                    vk::WriteDescriptorSet setWrite {};
                    setWrite.setDescriptorCount( 1 );
                    setWrite.setDescriptorType( sceneDataDescType );
                    setWrite.setDstBinding( 1 );
                    setWrite.setDstSet( __ubos._set );
                    setWrite.setBufferInfo( bufferInfos[bindingNum] );
                    setWrites[bindingNum] = std::move( setWrite );
                }
                __device.updateDescriptorSets( setWrites, nullptr );
            }


            // for( auto& b : __ubos ) {
            //     vk::DescriptorSetAllocateInfo allocInfo {};
            //     allocInfo.setDescriptorPool( __setPool );
            //     allocInfo.setDescriptorSetCount( 1 );
            //     allocInfo.setSetLayouts( *setLayout );
            //     b._set = __device.allocateDescriptorSets( allocInfo ).front();

            //     vk::DescriptorBufferInfo bufferInfo {};
            //     bufferInfo.setBuffer( b._buffer.buffer() );
            //     bufferInfo.setOffset( 0 );
            //     bufferInfo.setRange( totalSize );

            //     vk::WriteDescriptorSet setWrite {};
            //     setWrite.setDescriptorCount( 1 );
            //     setWrite.setDescriptorType( descriptorType );
            //     setWrite.setDstBinding( 0 );    // write into binding number 0
            //     setWrite.setDstSet( b._set );
            //     setWrite.setBufferInfo( bufferInfo );

            //     __device.updateDescriptorSets( setWrite, nullptr );
            // }
        }
    }

    /// Dynamic Uniform Buffers
    {

    }

    /// Another Descriptor Set creation
    {

    }
}

size_t Graphics::padUniformBufferSize(size_t originalSize) 
{
    // Calculate required alignment based on minimum device offset alignment
    size_t minUboAlignment = __physicalDevice.getProperties().limits.minUniformBufferOffsetAlignment;
    size_t alignedSize = originalSize;
    if (minUboAlignment > 0) {
        alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
    }
    return alignedSize;
}
