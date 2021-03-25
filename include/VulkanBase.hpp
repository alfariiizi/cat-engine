#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <memory>

#include "Window.hpp"
#include "Swapchain.hpp"
#include "Renderpass.hpp"
#include "DeletionQueue.hpp"
// #include "Command.hpp"
// #include "Synchronous.hpp"

namespace vma {
    typedef std::unique_ptr<vma::Allocator> UniqueAllocator;
}

#define MAX_FRAME 2

struct Descriptor
{
    vk::DescriptorPool      _pool;
    vk::DescriptorSet       _set;
};

class VulkanBase
{
public:
    VulkanBase() {}
    ~VulkanBase();

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
    const vk::PhysicalDevice&       getPhysicalDevice();
    const vk::SurfaceKHR&           getSurface();
    const vk::Device&               getDevice();
    const vma::Allocator&           getAllocator();
    const vk::Queue&                getGraphicsQueue();
    const vk::Queue&                getPresentQueue();
    Swapchain*                      getSwapchain();
    Renderpass*                     getRenderpass();

private:
    bool _hasBeenCreated_   = false;

    /* Main member variables */
private:
    vk::UniqueInstance                          _uInstance_;
    vk::DebugUtilsMessengerEXT                  _debugUtilsMessenger_;
    // vma::UniqueAllocator                        _allocator_;
    vma::Allocator                              _allocator_;
    vk::SurfaceKHR                              _surface_;
    vk::PhysicalDevice                          _physicalDevice_;
    vk::UniqueDevice                            _uDevice_;
    vk::Queue                                   _graphicsQueue_;
    vk::Queue                                   _presentQueue_;
    // Swapchain                                   _swapchain_;
    Renderpass                                  _renderpass_;
    std::unique_ptr<Swapchain>                  _swapchain_;
    std::unique_ptr<Renderpass>                 _renderpass_;
    DeletionQueue                               _delQueue_;

// private:
//     unsigned long        _timeOut                      = 1000000000;   // 1 second = 10^(9) nano second
};