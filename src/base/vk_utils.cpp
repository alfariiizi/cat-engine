#include "vk_utils.hpp"

#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VKAPI_ATTR VkBool32 VKAPI_CALL
    utils::debugUtilsMessengerCallback( VkDebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
                                VkDebugUtilsMessageTypeFlagsEXT              messageTypes,
                                VkDebugUtilsMessengerCallbackDataEXT const * pCallbackData,
                                void * /*pUserData*/ )
{
    std::cerr << vk::to_string( static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>( messageSeverity ) ) << ": "
            << vk::to_string( static_cast<vk::DebugUtilsMessageTypeFlagsEXT>( messageTypes ) ) << ":\n";
    std::cerr << "\t"
            << "messageIDName   = <" << pCallbackData->pMessageIdName << ">\n";
    std::cerr << "\t"
            << "messageIdNumber = " << pCallbackData->messageIdNumber << "\n";
    std::cerr << "\t"
            << "message         = <" << pCallbackData->pMessage << ">\n";
    if ( 0 < pCallbackData->queueLabelCount )
    {
    std::cerr << "\t"
                << "Queue Labels:\n";
    for ( uint8_t i = 0; i < pCallbackData->queueLabelCount; i++ )
    {
        std::cerr << "\t\t"
                << "labelName = <" << pCallbackData->pQueueLabels[i].pLabelName << ">\n";
    }
    }
    if ( 0 < pCallbackData->cmdBufLabelCount )
    {
    std::cerr << "\t"
                << "CommandBuffer Labels:\n";
    for ( uint8_t i = 0; i < pCallbackData->cmdBufLabelCount; i++ )
    {
        std::cerr << "\t\t"
                << "labelName = <" << pCallbackData->pCmdBufLabels[i].pLabelName << ">\n";
    }
    }
    if ( 0 < pCallbackData->objectCount )
    {
    std::cerr << "\t"
                << "Objects:\n";
    for ( uint8_t i = 0; i < pCallbackData->objectCount; i++ )
    {
        std::cerr << "\t\t"
                << "Object " << i << "\n";
        std::cerr << "\t\t\t"
                << "objectType   = "
                << vk::to_string( static_cast<vk::ObjectType>( pCallbackData->pObjects[i].objectType ) ) << "\n";
        std::cerr << "\t\t\t"
                << "objectHandle = " << pCallbackData->pObjects[i].objectHandle << "\n";
        if ( pCallbackData->pObjects[i].pObjectName )
        {
        std::cerr << "\t\t\t"
                    << "objectName   = <" << pCallbackData->pObjects[i].pObjectName << ">";
        }
    }

    std::cerr << "\n\n";
    }
    return VK_TRUE;
}

std::vector<const char*> utils::getValidationLayers() 
{
    return { "VK_LAYER_KHRONOS_validation" };
}

std::vector<const char*> utils::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

VkResult utils::CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger
)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if( func != nullptr )
    {
        return func( instance, pCreateInfo, pAllocator, pDebugMessenger );
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void utils::DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator
)
{
    auto func = ( PFN_vkDestroyDebugUtilsMessengerEXT )vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" );
    if( func != nullptr )
        func( instance, debugMessenger, pAllocator );
}

bool utils::IsDeviceSuitable(  const vk::PhysicalDevice& physicalDevice,
                        const vk::SurfaceKHR& surface )
{
    auto graphicsAndPresentQueueFamily = FindQueueFamilyIndices( physicalDevice, surface );
    return graphicsAndPresentQueueFamily.hasValue();
}

utils::QueueFamilyIndices utils::FindQueueFamilyIndices(  const vk::PhysicalDevice& physicalDevice,
                                            const vk::SurfaceKHR& surface )
{
    QueueFamilyIndices queueFamilyIndices;

    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
    size_t i = 0;
    for( const auto& q : queueFamilies )
    {
        // if graphics and present queue family are the same index
        if( q.queueCount > 0 
            && q.queueFlags & vk::QueueFlagBits::eGraphics
            && physicalDevice.getSurfaceSupportKHR( i, surface )
        )
        {
            queueFamilyIndices.graphicsFamily = i;
            queueFamilyIndices.presentFamily = i;
            break;
        }

        // if graphics and present family are not the same index
        else if( q.queueCount > 0
            && q.queueFlags & vk::QueueFlagBits::eGraphics )
        {
            queueFamilyIndices.graphicsFamily = i;
        }
        else if( physicalDevice.getSurfaceSupportKHR( i, surface ) )
        {
            queueFamilyIndices.presentFamily = i;
        }
        
        // if both graphics and present has been initialize
        if( queueFamilyIndices.hasValue() )
            break;
        ++i;
    }

    // if cannot find graphics and/or queue family that support vulkan
    if( !queueFamilyIndices.hasValue() )
    {
        throw std::runtime_error( "FAILED: Find Graphics and/or Queue Family Indices" );
    }

    return queueFamilyIndices;
}

vk::Extent2D utils::chooseSurfaceExtent( const vk::SurfaceCapabilitiesKHR& surfaceCapabilities, vk::Extent2D windowExtent ) 
{
    if( surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() )
        return surfaceCapabilities.currentExtent;
    else
    {
        vk::Extent2D actualExtent = windowExtent;
        
        actualExtent.width = std::clamp( actualExtent.width, 
                                            surfaceCapabilities.minImageExtent.width, 
                                            surfaceCapabilities.maxImageExtent.width );
        
        actualExtent.height = std::clamp( actualExtent.height, 
                                            surfaceCapabilities.minImageExtent.height, 
                                            surfaceCapabilities.maxImageExtent.height );
        
        return actualExtent;
    }
}

vk::SurfaceFormatKHR utils::chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surfaceFormats) 
{
    // if just found one surface format
    if( surfaceFormats.size() == 1 && surfaceFormats[0].format == vk::Format::eUndefined )
    {
        return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
    }

    // if found more than one surface format
    for( auto& surfaceFormat : surfaceFormats )
    {
        if( surfaceFormat.format == vk::Format::eB8G8R8A8Unorm 
            && surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear )
        {
            return surfaceFormat;
        }
    }

    // otherwise, just choose the avalible surface format
    return surfaceFormats[0];
}

vk::PresentModeKHR utils::choosePresentMode(const std::vector<vk::PresentModeKHR>& presentModes) 
{
    // choose the present mode
    for( auto& presentMode : presentModes )
    {
        if( presentMode == vk::PresentModeKHR::eFifo )
        {
            return presentMode;
        }
    }

    // otherwise, just choose the available present mode
    return presentModes[0];
}