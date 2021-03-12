#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include "Window.hpp"
#include "Swapchain.hpp"
#include "Renderpass.hpp"
// #include "Command.hpp"
// #include "Synchronous.hpp"

#define MAX_FRAME 2

struct Descriptor
{
    vk::DescriptorPool      _pool;
    vk::DescriptorSet       _set;
};

class VulkanBase
{
    /* Main member functions */
public:
    void init( Window& window );
    void destroy();

    /* Utils */
private:
    std::vector<const char*> instanceValidations();
    std::vector<const char*> instanceExtensions();

    /* Getter */
public:
    const vk::SurfaceKHR&           getSurface();
    const vk::Device&               getDevice();
    const vma::Allocator&           getAllocator();
    const vk::Queue&                getGraphicsQueue();
    const vk::Queue&                getPresentQueue();
    Swapchain                       getSwapchain();
    Renderpass                      getRenderpass();

private:
    bool _hasBeenCreated_   = false;

    /* Main member variables */
private:
    vk::Instance                        _instance_;
    vk::DebugUtilsMessengerEXT          _debugUtilsMessenger_;
    vma::Allocator                      _allocator_;
    vk::SurfaceKHR                      _surface_;
    vk::PhysicalDevice                  _physicalDevice_;
    vk::Device                          _device_;
    vk::Queue                           _graphicsQueue_;
    vk::Queue                           _presentQueue_;
    Swapchain                           _swapchain_;
    Renderpass                          _renderpass_;

// private:
//     unsigned long        _timeOut                      = 1000000000;   // 1 second = 10^(9) nano second
};