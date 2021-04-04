#pragma once

#include "vku/vku.hpp"
#include "ShaderStruct.hpp"
#include <string>
#include "DeletionQueue.hpp"


struct Pipeline
{
    vk::Pipeline _pipeline;
    vk::PipelineLayout _layout;
};

struct Descriptor
{
    vk::DescriptorSetLayout _layout;
    vk::DescriptorSet _set;
};

class Material
{
public:
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

    // struct State
    // {
    //     vk::Pipeline _pipeline;
    //     vk::PipelineLayout _layout;
    //     vk::DescriptorSetLayout _setLayout;
    //     vk::DescriptorSet _set;
    //     Texture _texture;
    //     vk::Sampler _sampler;
    //     std::string _name;
    // };

public:
    Material(   vk::PhysicalDevice      physicalDevice, 
                vk::Device              device, 
                vk::RenderPass          renderpass, 
                uint32_t                queueIndex, 
                vk::Queue               queue, 
                uint32_t                width, 
                uint32_t                height );
    ~Material();
    // void init( vk::PhysicalDevice physicalDevice, vk::Device device, vk::RenderPass renderpass, uint32_t queueIndex, vk::Queue queue, uint32_t width, uint32_t height );
    void destroy();
    // void loadTexture();
    void basicTexturedPipeline();

public:
    Pipeline* getPipeline( const std::string& name );
    Descriptor* getDescriptor( const std::string& name );

private:
    void createTexture();
    void createDescriptorSet();
    void createPipeline();

private:
    bool loadTexture( std::string filename, std::string textureName, ImageUsedFor usedFor );
    bool loadImageFromFile( const std::string& filename, vku::GenericImage* image, ImageUsedFor usedFor );

private:
    std::unordered_map<std::string, vku::TextureImage2D> loadedTexture2D;
    std::unordered_map<std::string, vku::TextureImageCube> loadedTextureCube;

private:
    vk::UniqueDescriptorPool __pDescriptorPool;
    vk::UniqueCommandPool __pCommandPool;

private:
    std::unordered_map<std::string, Pipeline> __pipelines;
    std::unordered_map<std::string, Descriptor> __descriptors;


    std::string __vertexShaderPath;
    std::string __fragmentShaderPath;
    std::string __assetsPath;

private:
    vk::PhysicalDevice __physicalDevice;
    vk::Device __device;
    vk::RenderPass __renderpass;
    uint32_t __transferQueueFamilyIndex;
    vk::Queue __transferQueue; // it almost always become a graphics queue for now
    vk::Extent2D __extent;

private:
    DeletionQueue __delQueue;
};