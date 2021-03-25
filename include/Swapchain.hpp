#pragma once

#include <vulkan/vulkan.hpp>

#include "DeletionQueue.hpp"

class Swapchain
{
public:
    struct State
    {
        vk::SwapchainKHR                _swapchain;
        vk::Format                      _format;
        vk::Extent2D                    _extent;
        std::vector<vk::Image>          _images;
        std::vector<vk::ImageView>      _imageViews;
        uint32_t                        _graphicsQueueFamilyIndices;
        uint32_t                        _presentQueueFamilyIndices;
    };

public:
    Swapchain() {};
    ~Swapchain();
    State getAllMembers();

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
    std::vector<vk::ImageView>          getImageViews();
    uint32_t                            getGraphicsQueueFamilyIndices();
    uint32_t                            getPresentQueueFamilyIndices();

    /* Utilities */
private:
    vk::Extent2D            chooseSurfaceExtent( vk::SurfaceCapabilitiesKHR& surfaceCapabilities, vk::Extent2D windowExtent );
    vk::SurfaceFormatKHR    chooseSurfaceFormat( std::vector<vk::SurfaceFormatKHR>& surfaceFormats );
    vk::PresentModeKHR      choosePresentMode( std::vector<vk::PresentModeKHR>& presentModes );

    /* Main member variables */
private:
    vk::UniqueSwapchainKHR              _uhSwapchain_;
    vk::Format                          _format_;
    std::vector<vk::Image>              _images_;
    std::vector<vk::UniqueImageView>    _uImageViews_;
    vk::Extent2D                        _extent_;
    uint32_t                            _graphicsQueueFamilyIndices_;
    uint32_t                            _presentQueueFamilyIndices_;

    /* Checker */
private:
    bool _hasBeenInitialized_    = false;
    bool _hasBeenCreated_        = false;

    /* Depend */
private:
    vk::Device                 _device_;
    vk::PhysicalDevice         _physicalDevice_;
    vk::SurfaceKHR             _surface_;
    DeletionQueue              _delQueue_;
};
