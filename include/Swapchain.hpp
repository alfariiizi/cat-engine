#pragma once

#include <vulkan/vulkan.hpp>

class Swapchain
{
    /* Main member function */
public:
    void init( vk::PhysicalDevice physicalDevice, const vk::SurfaceKHR& surface ,const vk::Device& device, const vk::Extent2D& extent );
    void create();
    void destroy();

    /* Function that used in Main member function */
private:
    void retrieveImagesAndCreateImageViews();

    /* Getter */
public:
    const vk::SwapchainKHR&             getSwapchain();
    const vk::Format&                   getFormat();
    const vk::Extent2D&                 getExtent();
    const std::vector<vk::Image>&       getImages();
    const std::vector<vk::ImageView>&   getImageViews();
    uint32_t                            getGraphicsQueueFamilyIndices();
    uint32_t                            getPresentQueueFamilyIndices();

    /* Utilities */
private:
    vk::Extent2D            chooseSurfaceExtent( vk::SurfaceCapabilitiesKHR& surfaceCapabilities, vk::Extent2D windowExtent );
    vk::SurfaceFormatKHR    chooseSurfaceFormat( std::vector<vk::SurfaceFormatKHR>& surfaceFormats );
    vk::PresentModeKHR      choosePresentMode( std::vector<vk::PresentModeKHR>& presentModes );

    /* Main member variables */
private:
    vk::SwapchainKHR            _hSwapchain_;
    vk::Format                  _format_;
    std::vector<vk::Image>      _images_;
    std::vector<vk::ImageView>  _imageViews_;
    vk::Extent2D                _extent_;
    uint32_t                    _graphicsQueueFamilyIndices_;
    uint32_t                    _presentQueueFamilyIndices_;

    /* Checker */
private:
    bool _hasBeenInitialized_    = false;
    bool _hasBeenCreated_        = false;

    /* Depend */
private:
    const vk::Device*                   _pDevice_;
    vk::PhysicalDevice                  _physicalDevice_;
    const vk::SurfaceKHR*               _pSurface_;
};