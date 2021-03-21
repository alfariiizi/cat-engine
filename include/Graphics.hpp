#pragma once

#include <vulkan/vulkan.hpp>
#include "Material.hpp"
#include "Renderpass.hpp"

/**
 * Idk, but maybe it'll use other class such as Material, Buffer, Image, Descriptor, Mesh, etc. (each 'll be an array)
 * So, it's going to be a main class for all of that.
 */

class Graphics
{
private:
    struct Queue
    {
        vk::Queue _graphics;
        vk::Queue _present;
    };

public:
    Graphics(){};
    ~Graphics();
    void init( vk::PhysicalDevice physicalDevice, const vk::Device& device, const Renderpass& renderpass, uint32_t width, uint32_t height, uint32_t queueIndex, vk::Queue graphicsQueue, vk::Queue presentQueue );
    void create();
    void destroy();
    void draw( vk::CommandBuffer cmd, uint32_t imageIndex );

private:
    void giveCommand( vk::CommandBuffer cmd, uint32_t imageIndex, vk::RenderPassBeginInfo& rpBeginInfo );

private:
    bool _hasBeenInit_      = false;
    bool _hasBeenCreated_   = false;
    const vk::ClearColorValue clearColorValue = vk::ClearColorValue{ std::array<float, 4U>{0.1f, 0.2f, 0.2f, 1.0f} };
    const vk::ClearDepthStencilValue clearDepthValue = vk::ClearDepthStencilValue{ 1.0f };

private:
    Material material;

    /// just for testing
private:
    const std::vector<Vertex> vertices = {
        {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    };
    vku::HostVertexBuffer vertBuffer;

    /* Depend */
private:
    vk::PhysicalDevice      _physicalDevice_;
    const vk::Device*       _pDevice_;
    Renderpass              _renderpass_;
    Queue                   _queue_;
    uint32_t                _width_             = 0;
    uint32_t                _height_            = 0;
};