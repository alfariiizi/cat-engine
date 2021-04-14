#pragma once

#include <vulkan/vulkan.hpp>
#include "DeletionQueue.hpp"
#include "Material.hpp"
#include "Mesh.hpp"

#include "ShaderStruct.hpp"

/**
 * Idk, but maybe it'll use other class such as Material, Buffer, Image, Descriptor, Mesh, etc. (each 'll be an array)
 * So, it's going to be a main class for all of that.
 */
// template<typename T>
class ObjectDraw
{
public:
    ObjectDraw() = default;
    ObjectDraw( Mesh* mesh, Material* material ) : __mesh( mesh ), __material( material ) {};
    Mesh* pMesh() { return __mesh; };
    Material* pMaterial() { return __material; };

private:
    Mesh* __mesh;
    Material* __material;
};


class Graphics
{
private:
    struct Queue
    {
        vk::Queue _graphics;
        vk::Queue _present;
    };

public:
    Graphics(   vk::PhysicalDevice              pd, 
                vk::Device                      device, 
                vma::Allocator                  allocator,
                vk::RenderPass                  rp,
                std::vector<vk::Framebuffer>    fbs, 
                const vk::Extent2D&             extent,
                uint32_t                        queueIndex,
                vk::Queue                       queue );
    ~Graphics();
    void draw( vk::CommandBuffer cmd, uint32_t imageIndex, uint32_t frameNumber );

private:
    void giveCommand( vk::CommandBuffer& cmd, uint32_t& imageIndex, uint32_t& frameNumber, vk::RenderPassBeginInfo& rpBeginInfo );
    void loadObject();

private:
    const vk::ClearColorValue clearColorValue = vk::ClearColorValue{ std::array<float, 4U>{0.1f, 0.2f, 0.2f, 1.0f} };
    const vk::ClearDepthStencilValue clearDepthValue = vk::ClearDepthStencilValue{ 1.0f };

private:
    Materials __material;
    std::vector<ObjectDraw> __objectDraw;

    /// just for testing
private:
    Mesh __triangleMesh;
    Mesh __monkey;

private:
    std::string __assetsPath;

    /* Depend */
private:
    vk::PhysicalDevice              __physicalDevice;
    vk::Device                      __device;
    vma::Allocator                  __allocator;
    vk::RenderPass                  __renderpass;
    std::vector<vk::Framebuffer>    __framebuffers;
    uint32_t                        __queueFamilyIndex;
    vk::Queue                       __queue;
    vk::Extent2D                    __extent;
    DeletionQueue                   __delQueue;
};