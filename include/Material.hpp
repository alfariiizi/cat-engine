#pragma once

#include "vku/vku.hpp"
#include "ShaderStruct.hpp"
#include <string>

class Material
{
public:
    struct State
    {
        vk::Pipeline _pipeline;
        vk::PipelineLayout _layout;
        vk::DescriptorSetLayout _setLayout;
        std::string _name;
    };

public:
    Material(){};
    ~Material();
    void init( vk::PhysicalDevice physicalDevice, vk::Device device, vk::RenderPass renderpass, uint32_t width, uint32_t height );
    void destroy();
    void trianglePipeline();
    std::vector<State>::iterator getMaterial( const std::string& name );

private:
    bool _isInitialized_ = false;
    bool _stillExist_ = false;
    uint32_t _width_ = 0;
    uint32_t _height_ = 0;

private:
    std::vector<State> _s_;
    std::string _vertexShaderPath_;
    std::string _fragmentShaderPath_;

private:
    vk::PhysicalDevice _physicalDevice_;
    vk::Device _device_;
    vk::RenderPass _renderpass_;
};