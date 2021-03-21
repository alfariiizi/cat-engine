#pragma once

#include "vku/vku.hpp"
#include "ShaderStruct.hpp"
#include <string>


class Material
{
public:
    struct Descriptor
    {
        
    };

    enum class ImageUsedFor
    {
        TEXTURE_2D,
        TEXTURE_CUBE
    };

    struct Texture
    {
        vku::TextureImage2D _textureImage2D;
        vku::TextureImageCube _textureCube;
    };

    struct State
    {
        vk::Pipeline _pipeline;
        vk::PipelineLayout _layout;
        vk::DescriptorSetLayout _setLayout;
        vk::DescriptorSet _set;
        Texture _texture;
        vk::Sampler _sampler;
        std::string _name;
    };

public:
    Material(){};
    ~Material();
    void init( vk::PhysicalDevice physicalDevice, vk::Device device, vk::RenderPass renderpass, uint32_t queueIndex, vk::Queue queue, uint32_t width, uint32_t height );
    void destroy();
    // void loadTexture();
    void trianglePipeline();
    void basicTexturedPipeline();
    std::vector<State>::iterator getMaterial( const std::string& name );

private:
    bool loadTexture( std::string filename, std::string textureName, ImageUsedFor usedFor );
    bool loadImageFromFile( const std::string& filename, vku::GenericImage* image, ImageUsedFor usedFor );

private:
    bool _isInitialized_ = false;
    bool _stillExist_ = false;
    uint32_t _width_ = 0;
    uint32_t _height_ = 0;

private:
    std::unordered_map<std::string, vku::TextureImage2D> loadedTexture2D;
    std::unordered_map<std::string, vku::TextureImageCube> loadedTextureCube;


private:
    vk::DescriptorPool _descriptorPool_;

private:
    std::vector<State> _s_;
    vk::CommandPool _cmdPool_;
    
    std::string _vertexShaderPath_;
    std::string _fragmentShaderPath_;
    std::string _assetsPath_;

private:
    vk::PhysicalDevice _physicalDevice_;
    vk::Device _device_;
    vk::RenderPass _renderpass_;
    vk::Queue _queue_; // it almost always become a graphics queue for now
};