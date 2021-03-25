#pragma once

#include <vulkan/vulkan.hpp>

#include "QueueFamilyIndices.hpp"
#include "DeletionQueue.hpp"

class Command
{
public:
    Command() {}
    ~Command();
    void init(  const vk::Device&       device, 
                uint32_t                graphicsQueueFamilyIndices, 
                uint32_t                bufferSize, 
                vk::CommandBufferLevel  bufferLevel );
    void create();
    void destroy();

public:
    const vk::CommandPool&                  getPool();
    const std::vector<vk::CommandBuffer>&   getBuffers();


private:
    vk::UniqueCommandPool                   _uPool_;
    std::vector<vk::UniqueCommandBuffer>    _uBuffers_;
    uint32_t                                _bufferSize_;
    vk::CommandBufferLevel                  _bufferLevel_;

private:
    bool _hasBeenInit_      = false;
    bool _hasBeenCreated_   = false;

private:
    vk::Device          _device_;
    uint32_t            _graphicsQueueFamilyIndices_;
    DeletionQueue       _delQueue_;
};