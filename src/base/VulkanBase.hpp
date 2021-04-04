#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include "vku/vku.hpp"
#include "Window.hpp"
#include "DeletionQueue.hpp"
#include "vma/vk_mem_alloc.hpp"

#define MAX_FRAME 2

struct Queue
{
    vk::Queue _graphics;
    vk::Queue _present;
};

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
    vk::Instance                    instance() const            { return __pInstance.get(); };
    vma::Allocator                  allocator() const           { return __allocator; };
    vk::PhysicalDevice              physicalDevice() const      { return __physicalDevice; };
    vk::Device                      device() const              { return __pDevice.get(); };
    vk::SurfaceKHR                  surface() const             { return __surface; };
    Queue                           graphicsPresentQueue()      { return { __graphicsQueue, __presentQueue }; };
    vk::SwapchainKHR                swapchain() const           { return __swapchain; };
    vk::Format                      swapchainFormat() const     { return __scFormat; };
    std::vector<vk::Image>          swapchainImages() const     { return __scImages; };
    std::vector<vk::ImageView>      swapchainImageViews() const { return __scImageViews; };
    vk::RenderPass                  renderpass() const          { return __renderpass; };
    std::vector<vk::Framebuffer>    framebuffers()              { return __rpFramebuffers; };
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

private:
//Device
    vk::SurfaceKHR                      __surface;
    vk::PhysicalDevice                  __physicalDevice;
    vk::UniqueDevice                    __pDevice;
//Queue
    vk::Queue                           __graphicsQueue;
    vk::Queue                           __presentQueue;
//Swapchain
    vk::SwapchainKHR                    __swapchain;
    vk::Format                          __scFormat;
    std::vector<vk::Image>              __scImages;
    std::vector<vk::ImageView>          __scImageViews;
//Renderpass
    vk::RenderPass                      __renderpass;
    vku::DepthStencilImage              __prpDepth;
    std::vector<vk::Framebuffer>        __rpFramebuffers;
};