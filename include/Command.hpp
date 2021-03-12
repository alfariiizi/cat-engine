#pragma once

#include <vulkan/vulkan.hpp>
#include "QueueFamilyIndices.hpp"

class Command
{
public:
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
    vk::CommandPool                     _pool_;
    std::vector<vk::CommandBuffer>      _buffers_;
    uint32_t                            _bufferSize_;
    vk::CommandBufferLevel              _bufferLevel_;

private:
    bool _hasBeenInit_      = false;
    bool _hasBeenCreated_   = false;

private:
    const vk::Device*   _pDevice_;
    uint32_t            _graphicsQueueFamilyIndices_;

};