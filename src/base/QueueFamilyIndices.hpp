#pragma once

#include <vulkan/vulkan.hpp>
#include <optional>

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

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
    static QueueFamilyIndices FindQueueFamilyIndices( const vk::PhysicalDevice& physicalDevice,
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
};