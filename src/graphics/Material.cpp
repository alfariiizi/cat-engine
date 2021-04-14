#include "Material.hpp"
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

typedef MeshPushConstants PushConstants;

Materials::Materials(vk::PhysicalDevice      physicalDevice, 
                vk::Device              device, 
                vk::RenderPass          renderpass, 
                uint32_t                queueIndex, 
                vk::Queue               queue, 
                uint32_t                width, 
                uint32_t                height) 
    :
    __physicalDevice( physicalDevice ),
    __device( device ),
    __renderpass( renderpass ),
    __transferQueueFamilyIndex( queueIndex ),
    __transferQueue( queue ),
    __extent( width, height )
{
    std::string sourceWorkspace = getenv( "PWD" );
    sourceWorkspace.append("/..");

    __vertexShaderPath.append( sourceWorkspace + "/shaders/spv/");
    __fragmentShaderPath = __vertexShaderPath;
    __vertexShaderPath.append( "vertex" );
    __fragmentShaderPath.append( "fragment" );

    __assetsPath = sourceWorkspace;
    __assetsPath.append( "/assets/" );

    { /// Command Pool
        vk::CommandPoolCreateInfo poolInfo {};
        poolInfo.setQueueFamilyIndex( queueIndex );
        poolInfo.setFlags( vk::CommandPoolCreateFlagBits::eResetCommandBuffer );

        __pCommandPool = __device.createCommandPoolUnique( poolInfo );
    }

    { /// Descriptor Pool
        std::vector<vk::DescriptorPoolSize> sizes = {
            { vk::DescriptorType::eCombinedImageSampler, 100U },
            { vk::DescriptorType::eSampledImage, 100U },
            { vk::DescriptorType::eSampler, 100U },
            { vk::DescriptorType::eStorageBuffer, 100U },
            { vk::DescriptorType::eStorageBufferDynamic, 100U },
            { vk::DescriptorType::eStorageImage, 100U },
            { vk::DescriptorType::eStorageTexelBuffer, 100U },
            { vk::DescriptorType::eUniformBuffer, 100U },
            { vk::DescriptorType::eUniformBufferDynamic, 100U },
            { vk::DescriptorType::eUniformTexelBuffer, 100U },
        };

        auto descPoolInfo = vk::DescriptorPoolCreateInfo {
            vk::DescriptorPoolCreateFlags(),
            10U,
            static_cast<uint32_t>(sizes.size()), sizes.data()
        };

        __pDescriptorPool = __device.createDescriptorPoolUnique( descPoolInfo );
    }

    createTexture();
    createDescriptorSet();
    createPipeline();
}

Materials::~Materials() 
{
    __delQueue.flush();
    std::cout << "Call Material Desctructor\n";
}

Pipeline* Materials::getPipeline( const std::string& name )
{
    auto result = __pipelines.find( name );
    if( result == __pipelines.end() )
        return nullptr;

    return &result->second;
}

Descriptor* Materials::getDescriptor( const std::string& name )
{
    auto result = __descriptors.find( name );
    if( result == __descriptors.end() )
        return nullptr;

    return &result->second;
}

// void Materials::destroy() 
// {
//     assert( _isInitialized_ );

//     if( _stillExist_ )
//     {
//         for( auto& s : _s_ )
//         {
//             __device.destroyPipeline( s._pipeline );
//             __device.destroyPipelineLayout( s._layout );
//             // __device.destroyDescriptorSetLayout( s._setLayout );
//         }
//     }

//     _stillExist_ = false;
// }

// void Materials::loadTexture() 
// {
//     /// BRICK WALL
//     {
//         /// Image file name, Texture name, and Texture used (CHANGEABLE)
//         std::string filename = "wall.jpg";
//         std::string textureName = "bricks_of_wall";
//         ImageUsedFor usedFor = ImageUsedFor::TEXTURE_2D;

//         /// Loading the texture (FIX)
//         if( usedFor == ImageUsedFor::TEXTURE_2D ) {
//             vku::TextureImage2D texture;
//             bool success = loadImageFromFile( __assetsPath + filename, &texture, usedFor );
//             if( !success )
//                 throw std::runtime_error( "Failed to load the image" );
//             loadedTexture2D[textureName] = std::move(texture);
//         }
//         else if( usedFor == ImageUsedFor::TEXTURE_CUBE ) {
//             vku::TextureImageCube texture;
//             bool success = loadImageFromFile( __assetsPath + filename, &texture, usedFor );
//             if( !success )
//                 throw std::runtime_error( "Failed to load the image" );
            
//             loadedTextureCube[textureName] = std::move( texture );
//         }
//         else {}
//     }

//     /// ANOTHER TEXTURE
//     {
        
//     }
// }


// void Materials::basicTexturedPipeline() 
// {
//     /// State (FIX)
//     State state;
//     vku::PipelineMaker plm { _width_, _height_ };

//     /// Descriptor's Things (in this case: Texture)
//     vk::Sampler sampler;
//     {
//         // loading the texture
//         std::string filename = "wall.jpg";
//         std::string textureName = "bricks_of_wall";
//         ImageUsedFor usedFor = ImageUsedFor::TEXTURE_2D;

//         /// Loading the texture (FIX)
//         bool success = loadImageFromFile( __assetsPath + filename, &state._texture._textureImage2D, usedFor );
//         if( !success )
//             throw std::runtime_error( "Failed to load the image" );

//         // Descriptor layout
//         vku::DescriptorSetLayoutMaker dslm {};
//         // dslm.buffer( 0U, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 1 );
//         dslm.image( 0U, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, 1 );
//         state._setLayout = dslm.createUnique( __device ).release();

//         vku::SamplerMaker samplerMaker {};
//         sampler = samplerMaker.create( __device );

//         vku::DescriptorSetMaker dsm {};
//         dsm.layout( state._setLayout );
//         state._set = dsm.create( __device, __descriptorPool ).front();

//         vku::DescriptorSetUpdater updater {0, 1, 0};
//         updater.beginDescriptorSet( state._set );
//         updater.beginImages( 0, 0, vk::DescriptorType::eCombinedImageSampler );
//         updater.image( sampler, state._texture._textureImage2D.imageView(), vk::ImageLayout::eShaderReadOnlyOptimal );
//         // updater.buffer()
//         // updater.update( __device );
//     }

//     /// Layout (CHANGEABLE)
//     {
//         auto layoutMaker = vku::PipelineLayoutMaker{};
//         layoutMaker.descriptorSetLayout( state._setLayout );
//         state._layout = layoutMaker.createUnique( __device ).release();
//     }


//     /// Shader (CHANGEABLE)
//     std::string filename = "simpleTexture";

//     filename.append(".spv");
//     std::string vertFilename = __vertexShaderPath + "/" + filename;
//     std::string fragFilename = __fragmentShaderPath + "/" + filename;

//     vku::ShaderModule vertexShader { __device, vertFilename };
//     vku::ShaderModule fragmentShader { __device, fragFilename };
//     if( !vertexShader.ok() || !fragmentShader.ok() )
//         throw std::runtime_error( "FAILED: creating shaders" );
//     plm.shader( vk::ShaderStageFlagBits::eVertex, vertexShader );
//     plm.shader( vk::ShaderStageFlagBits::eFragment, fragmentShader );


//     /// Pipeline State (CHANGEABLE)
//     {
//         // Depth
//         plm.depthTestEnable( VK_TRUE ).depthWriteEnable( VK_TRUE );

//         // Vertex Input
//         plm.vertexBinding( 0, sizeof(Vertex), vk::VertexInputRate::eVertex );
//         plm.vertexAttribute( 0, 0, vk::Format::eR32G32B32Sfloat, offsetof( Texture_Vertex, position ) );
//         plm.vertexAttribute( 1, 0, vk::Format::eR32G32Sfloat, offsetof( Texture_Vertex, uv ) );

//     }

//     /// Create the pipeline (FIX)
//     state._pipeline = plm.createUnique( __device, nullptr, state._layout, __renderpass ).release();

//     /// Naming the pipeline (CHANGEABLE)
//     state._name = "basicTextured";

//     /// Copying the pipeline (FIX)
//     _s_.emplace_back( std::move(state) );
// }

Material* Materials::pMaterial(const std::string& name) 
{
    auto success = __loadedMaterials.find( name );
    if( success == __loadedMaterials.end() )
        return nullptr;
    
    return &(success->second);
}


void Materials::createTexture() 
{
    
}

void Materials::createDescriptorSet() 
{
    
}

void Materials::createPipeline() 
{
    /**
     * @brief Triangle
     */
    {
        vku::PipelineMaker plm { __extent.width, __extent.height };
        vk::Pipeline pipeline;
        vk::PipelineLayout pipelineLayout;

        /// Layout (CHANGEABLE)
        {
            auto layoutMaker = vku::PipelineLayoutMaker{};
            layoutMaker.pushConstantRange( vk::ShaderStageFlagBits::eVertex, 0, sizeof( PushConstants ) );

            pipelineLayout = layoutMaker.createUnique( __device ).release();
            __delQueue.pushFunction([d=__device, pl=pipelineLayout](){
                d.destroyPipelineLayout( pl );
            });
        }


        /// Shader (CHANGEABLE)
        std::string filename = "pushConstantTriangle";

        filename.append(".spv");
        std::string vertFilename = __vertexShaderPath + "/" + filename;
        std::string fragFilename = __fragmentShaderPath + "/" + filename;

        vku::ShaderModule vertexShader { __device, vertFilename };
        vku::ShaderModule fragmentShader { __device, fragFilename };
        if( !vertexShader.ok() || !fragmentShader.ok() )
            throw std::runtime_error( "FAILED: creating shaders" );
        plm.shader( vk::ShaderStageFlagBits::eVertex, vertexShader );
        plm.shader( vk::ShaderStageFlagBits::eFragment, fragmentShader );


        /// Pipeline State (CHANGEABLE)
        auto vertexDescription = Vertex::SimpleVertex::getVertexInputDescription();
        {
            // Depth
            plm.depthTestEnable( VK_TRUE ).depthWriteEnable( VK_TRUE );

            // Vertex Description
            for( auto& binding : vertexDescription.bindings )
                plm.vertexBinding( binding );
            for( auto& attribute : vertexDescription.attributs )
                plm.vertexAttribute( attribute );
        }

        /// Create the pipeline (FIX)
        pipeline = plm.createUnique( __device, nullptr, pipelineLayout, __renderpass ).release();
        __delQueue.pushFunction( [d=__device, p=pipeline](){
            d.destroyPipeline( p );
        });

        /// Naming the pipeline (CHANGEABLE)
        __loadedMaterials["triangle"] = { pipeline, pipelineLayout };
    }


    /// Another Pipeline
    {

    }

}

bool Materials::loadTexture(std::string filename, std::string textureName, ImageUsedFor usedFor) 
{
    /// Loading the texture (FIX)
    bool isSuccess = false;
    if( usedFor == ImageUsedFor::TEXTURE_2D ) {
        vku::TextureImage2D texture;
        bool isSuccess = loadImageFromFile( __assetsPath + filename, &texture, usedFor );
        loadedTexture2D[textureName] = std::move(texture);
        isSuccess = true;
    }
    else if( usedFor == ImageUsedFor::TEXTURE_CUBE ) {
        vku::TextureImageCube texture;
        bool success = loadImageFromFile( __assetsPath + filename, &texture, usedFor );
        loadedTextureCube[textureName] = std::move( texture );
        isSuccess = true;
    }
    else {}

    return isSuccess;
}

bool Materials::loadImageFromFile( const std::string& filename, vku::GenericImage* image, ImageUsedFor usedFor ) 
{
    // assert( _isInitialized_ );

    int texWidth, texHeight, texChannel;
    auto desiredChannel = STBI_rgb_alpha;

    stbi_uc* pixels = stbi_load( filename.c_str(), &texWidth, &texHeight, &texChannel, desiredChannel );

    if( !pixels )
        return false;

    vk::DeviceSize imageSize = texWidth * texHeight * static_cast<int>(desiredChannel);
    vk::Format textureFormat = vk::Format::eR8G8B8A8Srgb;

    std::vector<uint8_t> bytes { pixels, pixels + static_cast<int>(imageSize) - 1 };

    switch (usedFor)
    {
    case ImageUsedFor::TEXTURE_2D :
        *image = vku::TextureImage2D { __device, __physicalDevice.getMemoryProperties(), 
                                        static_cast<uint32_t>(texWidth),
                                        static_cast<uint32_t>(texHeight), 
                                        1U, textureFormat };
        break;
    case ImageUsedFor::TEXTURE_CUBE :
        *image = vku::TextureImageCube { __device, __physicalDevice.getMemoryProperties(), 
                                            static_cast<uint32_t>(texWidth),
                                            static_cast<uint32_t>(texHeight), 
                                            1U, textureFormat };
        break;
    default:
        break;
    }

    image->upload( __device, bytes, __pCommandPool.get(), __physicalDevice.getMemoryProperties(), __transferQueue );

    return true;
}