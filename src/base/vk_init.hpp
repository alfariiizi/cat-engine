#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace init
{
// Non Unique
vk::Instance        createInstance                  ();
void                createDebugUtilsMessengerInfo   ( const vk::Instance& instance, VkDebugUtilsMessengerEXT& outDebugUtilsMessenger );
vk::SurfaceKHR      createSurfce                    ( const vk::Instance& instance, GLFWwindow* window );
vk::PhysicalDevice  pickPhysicalDevice              ( const vk::Instance& instance, const vk::SurfaceKHR& surface );

// Unique
vk::UniqueDevice        createDevice                    ( const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface );   
vk::UniqueSwapchainKHR  createSwapchain                 ( const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const vk::SurfaceKHR& surface, vk::Extent2D windowExtent );
void                    swapchainImageAndImageViews ( const vk::Device& device, const vk::SwapchainKHR& swapchain, vk::Format swapchainFormat, std::vector<vk::Image>& outSwapchainImages, std::vector<vk::UniqueImageView>& outSwapchainImageViews );

// Maybe will be Unique in future
vk::CommandPool     createCommandPool( const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const vk::Device& device );
std::vector<vk::CommandBuffer> createCommandBuffers( const vk::Device& device, const vk::CommandPool& cmdPool, vk::CommandBufferLevel level, uint32_t count );

} // namespace init
