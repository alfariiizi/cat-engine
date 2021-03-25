#pragma once

#include <vulkan/vulkan.hpp>
#include <optional>
// #include "vk_mem_alloc.hpp"

// struct Image
// {
//     vma::Allocation _memory;
//     vk::Image _image;
// };

struct Buffer
{
    vma::Allocation _memory;
    vk::Buffer _buffer;
};

namespace utils
{
struct QueueFamilyIndices
{
    bool hasValue()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
    bool exslusive()
    {
        return graphicsFamily.value() == presentFamily.value();
    }
    std::vector<uint32_t> graphicsAndPresentFamilyIndex()
    {
        return { graphicsFamily.value(), presentFamily.value() };
    }

    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
};
VkResult 
    CreateDebugUtilsMessengerEXT(
                                 VkInstance instance,
                                 VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                 const VkAllocationCallbacks* pAllocator,
                                 VkDebugUtilsMessengerEXT* pDebugMessenger );
VKAPI_ATTR VkBool32 VKAPI_CALL
    debugUtilsMessengerCallback( VkDebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
                                VkDebugUtilsMessageTypeFlagsEXT              messageTypes,
                                VkDebugUtilsMessengerCallbackDataEXT const * pCallbackData,
                                void * /*pUserData*/ );
std::vector<const char*> getValidationLayers();
std::vector<const char*> getRequiredExtensions();
void DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator
);
bool IsDeviceSuitable(  const vk::PhysicalDevice& physicalDevice,
                        const vk::SurfaceKHR& surface );
QueueFamilyIndices FindQueueFamilyIndices(  const vk::PhysicalDevice& physicalDevice,
                                            const vk::SurfaceKHR& surface );
vk::Extent2D chooseSurfaceExtent( vk::SurfaceCapabilitiesKHR& surfaceCapabilities, vk::Extent2D windowExtent );
vk::SurfaceFormatKHR chooseSurfaceFormat( std::vector<vk::SurfaceFormatKHR>& surfaceFormats );
vk::PresentModeKHR choosePresentMode( std::vector<vk::PresentModeKHR>& presentModes );
std::vector<char> readFile( const std::string& fileName );
vk::UniqueShaderModule createShaderModule( const vk::Device& device ,const std::vector<char>& code );

} // namespace utils
