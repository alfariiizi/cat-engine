#include "Material.hpp"
#include <stdio.h>

#include "ShaderStruct.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


Material::~Material() 
{
    destroy();
}

void Material::init( vk::PhysicalDevice physicalDevice, vk::Device device, vk::RenderPass renderpass, uint32_t queueIndex, vk::Queue queue, uint32_t width, uint32_t height )
{
    _physicalDevice_ = physicalDevice;
    _device_ = device;
    _renderpass_ = renderpass;
    _width_ = width;
    _height_ = height;
    _queue_ = queue;

    std::string sourceWorkspace = getenv( "PWD" );
    sourceWorkspace.append("/..");

    _vertexShaderPath_.append( sourceWorkspace + "/shaders/spv/");
    _fragmentShaderPath_ = _vertexShaderPath_;
    _vertexShaderPath_.append( "vertex" );
    _fragmentShaderPath_.append( "fragment" );

    _assetsPath_ = sourceWorkspace;
    _assetsPath_.append( "/assets/" );

    {
        vk::CommandPoolCreateInfo poolInfo {};
        poolInfo.setQueueFamilyIndex( queueIndex );
        poolInfo.setFlags( vk::CommandPoolCreateFlagBits::eResetCommandBuffer );

        _cmdPool_ = _device_.createCommandPool( poolInfo );
    }

    /// Descriptor Pool
    {
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

        _descriptorPool_ = _device_.createDescriptorPool( descPoolInfo );
    }

    trianglePipeline();
    basicTexturedPipeline();

    _isInitialized_ = true;
    _stillExist_ = true;
}

void Material::destroy() 
{
    assert( _isInitialized_ );

    if( _stillExist_ )
    {
        for( auto& s : _s_ )
        {
            _device_.destroyPipeline( s._pipeline );
            _device_.destroyPipelineLayout( s._layout );
            // _device_.destroyDescriptorSetLayout( s._setLayout );
        }
    }

    _stillExist_ = false;
}

// void Material::loadTexture() 
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
//             bool success = loadImageFromFile( _assetsPath_ + filename, &texture, usedFor );
//             if( !success )
//                 throw std::runtime_error( "Failed to load the image" );
//             loadedTexture2D[textureName] = std::move(texture);
//         }
//         else if( usedFor == ImageUsedFor::TEXTURE_CUBE ) {
//             vku::TextureImageCube texture;
//             bool success = loadImageFromFile( _assetsPath_ + filename, &texture, usedFor );
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

void Material::trianglePipeline() 
{
    /// State (FIX)
    State state;
    vku::PipelineMaker plm { _width_, _height_ };

    /// Descriptor set layout (CHANGEABLE)
    {
    }

    /// Layout (CHANGEABLE)
    {
        auto layoutMaker = vku::PipelineLayoutMaker{};
        state._layout = layoutMaker.createUnique( _device_ ).release();
    }


    /// Shader (CHANGEABLE)
    std::string filename = "simpletriangle";

    filename.append(".spv");
    std::string vertFilename = _vertexShaderPath_ + "/" + filename;
    std::string fragFilename = _fragmentShaderPath_ + "/" + filename;

    vku::ShaderModule vertexShader { _device_, vertFilename };
    vku::ShaderModule fragmentShader { _device_, fragFilename };
    if( !vertexShader.ok() || !fragmentShader.ok() )
        throw std::runtime_error( "FAILED: creating shaders" );
    plm.shader( vk::ShaderStageFlagBits::eVertex, vertexShader );
    plm.shader( vk::ShaderStageFlagBits::eFragment, fragmentShader );


    /// Pipeline State (CHANGEABLE)
    auto vertexDesc = Vertex::getVertexInputDescription();
    {
        // Depth
        plm.depthTestEnable( VK_TRUE ).depthWriteEnable( VK_TRUE );

        plm.vertexBinding( vertexDesc.bindings.front() );
        // for( auto& bind : vertexDesc.bindings )
        //     plm.vertexBinding( bind );

        for( auto& attr : vertexDesc.attributs )
            plm.vertexAttribute( attr );
    }

    /// Create the pipeline (FIX)
    state._pipeline = plm.createUnique( _device_, nullptr, state._layout, _renderpass_ ).release();

    /// Naming the pipeline (CHANGEABLE)
    state._name = "triangle";

    /// Copying the pipeline (FIX)
    _s_.emplace_back( std::move(state) );
}

void Material::basicTexturedPipeline() 
{
    /// State (FIX)
    State state;
    vku::PipelineMaker plm { _width_, _height_ };

    /// Descriptor's Things (in this case: Texture)
    vk::Sampler sampler;
    {
        // loading the texture
        std::string filename = "wall.jpg";
        std::string textureName = "bricks_of_wall";
        ImageUsedFor usedFor = ImageUsedFor::TEXTURE_2D;

        /// Loading the texture (FIX)
        bool success = loadImageFromFile( _assetsPath_ + filename, &state._texture._textureImage2D, usedFor );
        if( !success )
            throw std::runtime_error( "Failed to load the image" );

        // Descriptor layout
        vku::DescriptorSetLayoutMaker dslm {};
        // dslm.buffer( 0U, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 1 );
        dslm.image( 0U, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, 1 );
        state._setLayout = dslm.createUnique( _device_ ).release();

        vku::SamplerMaker samplerMaker {};
        sampler = samplerMaker.create( _device_ );

        vku::DescriptorSetMaker dsm {};
        dsm.layout( state._setLayout );
        state._set = dsm.create( _device_, _descriptorPool_ ).front();

        vku::DescriptorSetUpdater updater {0, 1, 0};
        updater.beginDescriptorSet( state._set );
        updater.beginImages( 0, 0, vk::DescriptorType::eCombinedImageSampler );
        updater.image( sampler, state._texture._textureImage2D.imageView(), vk::ImageLayout::eShaderReadOnlyOptimal );
        // updater.buffer()
        // updater.update( _device_ );
    }

    /// Layout (CHANGEABLE)
    {
        auto layoutMaker = vku::PipelineLayoutMaker{};
        layoutMaker.descriptorSetLayout( state._setLayout );
        state._layout = layoutMaker.createUnique( _device_ ).release();
    }


    /// Shader (CHANGEABLE)
    std::string filename = "simpleTexture";

    filename.append(".spv");
    std::string vertFilename = _vertexShaderPath_ + "/" + filename;
    std::string fragFilename = _fragmentShaderPath_ + "/" + filename;

    vku::ShaderModule vertexShader { _device_, vertFilename };
    vku::ShaderModule fragmentShader { _device_, fragFilename };
    if( !vertexShader.ok() || !fragmentShader.ok() )
        throw std::runtime_error( "FAILED: creating shaders" );
    plm.shader( vk::ShaderStageFlagBits::eVertex, vertexShader );
    plm.shader( vk::ShaderStageFlagBits::eFragment, fragmentShader );


    /// Pipeline State (CHANGEABLE)
    {
        // Depth
        plm.depthTestEnable( VK_TRUE ).depthWriteEnable( VK_TRUE );

        // Vertex Input
        plm.vertexBinding( 0, sizeof(Vertex), vk::VertexInputRate::eVertex );
        plm.vertexAttribute( 0, 0, vk::Format::eR32G32B32Sfloat, offsetof( Texture_Vertex, position ) );
        plm.vertexAttribute( 1, 0, vk::Format::eR32G32Sfloat, offsetof( Texture_Vertex, uv ) );

    }

    /// Create the pipeline (FIX)
    state._pipeline = plm.createUnique( _device_, nullptr, state._layout, _renderpass_ ).release();

    /// Naming the pipeline (CHANGEABLE)
    state._name = "basicTextured";

    /// Copying the pipeline (FIX)
    _s_.emplace_back( std::move(state) );
}

std::vector<Material::State>::iterator Material::getMaterial(const std::string& name) 
{
    assert( _isInitialized_ );
    auto it = std::find_if( _s_.begin(), _s_.end(), 
                [&]( const State& state ){
                    return strcmp( state._name.c_str(), name.c_str() ) == 0;
                }
    );

    assert( it != _s_.end() );

    return it;
}

bool Material::loadTexture(std::string filename, std::string textureName, ImageUsedFor usedFor) 
{
    /// Loading the texture (FIX)
    bool isSuccess = false;
    if( usedFor == ImageUsedFor::TEXTURE_2D ) {
        vku::TextureImage2D texture;
        bool isSuccess = loadImageFromFile( _assetsPath_ + filename, &texture, usedFor );
        loadedTexture2D[textureName] = std::move(texture);
        isSuccess = true;
    }
    else if( usedFor == ImageUsedFor::TEXTURE_CUBE ) {
        vku::TextureImageCube texture;
        bool success = loadImageFromFile( _assetsPath_ + filename, &texture, usedFor );
        loadedTextureCube[textureName] = std::move( texture );
        isSuccess = true;
    }
    else {}

    return isSuccess;
}

bool Material::loadImageFromFile( const std::string& filename, vku::GenericImage* image, ImageUsedFor usedFor ) 
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
        *image = vku::TextureImage2D { _device_, _physicalDevice_.getMemoryProperties(), 
                                        static_cast<uint32_t>(texWidth),
                                        static_cast<uint32_t>(texHeight), 
                                        1U, textureFormat };
        break;
    case ImageUsedFor::TEXTURE_CUBE :
        *image = vku::TextureImageCube { _device_, _physicalDevice_.getMemoryProperties(), 
                                            static_cast<uint32_t>(texWidth),
                                            static_cast<uint32_t>(texHeight), 
                                            1U, textureFormat };
        break;
    default:
        break;
    }

    image->upload( _device_, bytes, _cmdPool_, _physicalDevice_.getMemoryProperties(), _queue_ );

    return true;
}
