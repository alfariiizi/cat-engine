#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include "vku/vku.hpp"
#include "Window.hpp"
#include "DeletionQueue.hpp"
#include "vma/vk_mem_alloc.hpp"

#define MAX_FRAME 2

class VulkanBase
{
    /* Main member functions */
public:
    VulkanBase( Window& window );
    ~VulkanBase();
    // void init( Window& window );
    // void destroy();

    /* Utils */
private:
    std::vector<const char*> instanceValidations();
    std::vector<const char*> instanceExtensions();

    /* Getter */
public:
    vk::Instance instance() { return __pInstance.get(); };
    vma::Allocator allocator() { return __allocator; };
    std::vector<vk::Framebuffer>    getFramebuffers();
    // const vk::PhysicalDevice&       getPhysicalDevice();
    // const vk::SurfaceKHR&           getSurface();
    // const vk::Device&               getDevice();
    // const vma::Allocator&           getAllocator();
    // const vk::Queue&                getGraphicsQueue();
    // const vk::Queue&                getPresentQueue();

private:
    bool _hasBeenCreated_   = false;

    /* Main member variables */
private:
    vk::UniqueInstance                  __pInstance;
    vk::DebugUtilsMessengerEXT          __debugUtilsMessenger;
    vma::Allocator                      __allocator;
    DeletionQueue                       __delQueue;

public:
//Device
    vk::SurfaceKHR                      _surface;
    vk::PhysicalDevice                  _physicalDevice;
    vk::UniqueDevice                    _pDevice;
//Queue
    vk::Queue                           _graphicsQueue;
    vk::Queue                           _presentQueue;
//Swapchain
    vk::UniqueSwapchainKHR              _pSwapchain;
    vk::Format                          _scFormat;
    std::vector<vk::Image>              _scImage;
    std::vector<vk::UniqueImageView>    _pscImageView;
//Renderpass
    vk::UniqueRenderPass                _pRenderpass;
    vku::DepthStencilImage              _prpDepth;
    std::vector<vk::UniqueFramebuffer>  _prpFramebuffers;
};