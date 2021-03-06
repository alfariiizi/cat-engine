#include "Swapchain.hpp"

#include "QueueFamilyIndices.hpp"

void Swapchain::init( vk::PhysicalDevice physicalDevice, const vk::SurfaceKHR& surface ,const vk::Device& device, const vk::Extent2D& extent ) 
{
    _physicalDevice_ = physicalDevice;
    _pSurface_ = &surface;
    _pDevice_ = &device;
    _extent_ = extent;

    _hasBeenInitialized_ = true;
}

void Swapchain::create() 
{
    assert( _hasBeenInitialized_ );

    auto surfaceCapability = _physicalDevice_.getSurfaceCapabilitiesKHR( *_pSurface_ );
    auto surfaceFormats = _physicalDevice_.getSurfaceFormatsKHR( *_pSurface_ );
    auto surfacePresentModes = _physicalDevice_.getSurfacePresentModesKHR( *_pSurface_ );

    auto surfaceExtent = chooseSurfaceExtent( surfaceCapability, _extent_ );
    auto surfaceFormat = chooseSurfaceFormat( surfaceFormats );
    auto presentMode = choosePresentMode( surfacePresentModes );

    uint32_t imageCount = surfaceCapability.minImageCount + 1;
    if (surfaceCapability.maxImageCount > 0 && imageCount > surfaceCapability.maxImageCount) {
        imageCount = surfaceCapability.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR swapchainInfo {
        {},
        *_pSurface_,
        imageCount,
        surfaceFormat.format,       // surfaceFormat.format == image format
        surfaceFormat.colorSpace,   // surfaceFormat.colorspace == image colorspace
        surfaceExtent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment,
        {},  // sharing mode
        {},  // queue family indices
        surfaceCapability.currentTransform,     // transform
        vk::CompositeAlphaFlagBitsKHR::eOpaque, // composite alpha
        presentMode,
        VK_TRUE,                                // clipped
        nullptr                                 // old swapchain
    };

    auto graphicsAndPresentQueueFamily = QueueFamilyIndices::FindQueueFamilyIndices( _physicalDevice_, *_pSurface_ );
    _graphicsQueueFamilyIndices_ = graphicsAndPresentQueueFamily.graphicsFamily.value();
    _presentQueueFamilyIndices_ = graphicsAndPresentQueueFamily.presentFamily.value();
    if( !graphicsAndPresentQueueFamily.exslusive() )
    {
        swapchainInfo.setImageSharingMode( vk::SharingMode::eConcurrent );
        swapchainInfo.setQueueFamilyIndices( graphicsAndPresentQueueFamily.graphicsAndPresentFamilyIndex().front() );
    }
    else
    {
        swapchainInfo.setImageSharingMode( vk::SharingMode::eExclusive );
        swapchainInfo.setQueueFamilyIndices( nullptr );
    }

    _hSwapchain_ = _pDevice_->createSwapchainKHR( swapchainInfo );
    _format_ = surfaceFormat.format;

    retrieveImagesAndCreateImageViews();

    _hasBeenCreated_ = true;
}

void Swapchain::destroy() 
{
    assert( _hasBeenCreated_ );

    for( auto imageView : _imageViews_ )
        _pDevice_->destroyImageView( imageView );

    _pDevice_->destroySwapchainKHR( _hSwapchain_ );

    _hasBeenCreated_ = false;
}

void Swapchain::retrieveImagesAndCreateImageViews() 
{
    _images_ = _pDevice_->getSwapchainImagesKHR( _hSwapchain_ );
    _imageViews_.reserve( _images_.size() );

    for( auto& image : _images_ )
    {
        vk::ImageViewCreateInfo imageViewInfo {
            vk::ImageViewCreateFlags(),
            image,
            vk::ImageViewType::e2D,
            _format_,
            vk::ComponentMapping{
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity,
            },
            vk::ImageSubresourceRange {
                vk::ImageAspectFlagBits::eColor,    // aspect flags
                0,      // base mip map
                1,      // level count
                0,      // base array layer
                1       // layer count
            }
        };
        _imageViews_.emplace_back( _pDevice_->createImageView( imageViewInfo ) );
    }
}

const vk::SwapchainKHR& Swapchain::getSwapchain() 
{
    assert( _hasBeenCreated_ );
    return _hSwapchain_;
}

const vk::Format& Swapchain::getFormat() 
{
    assert( _hasBeenCreated_ );
    return _format_;
}

const vk::Extent2D& Swapchain::getExtent() 
{
    assert( _hasBeenInitialized_ );
    return _extent_;
}

const std::vector<vk::Image>& Swapchain::getImages() 
{
    assert( _hasBeenCreated_ );
    return _images_;
}

const std::vector<vk::ImageView>& Swapchain::getImageViews() 
{
    assert( _hasBeenCreated_ );
    return _imageViews_;
}

uint32_t Swapchain::getGraphicsQueueFamilyIndices() 
{
    assert( _hasBeenCreated_ );
    return _graphicsQueueFamilyIndices_;
}

uint32_t Swapchain::getPresentQueueFamilyIndices() 
{
    assert( _hasBeenCreated_ );
    return _presentQueueFamilyIndices_;
}

vk::Extent2D Swapchain::chooseSurfaceExtent( vk::SurfaceCapabilitiesKHR& surfaceCapabilities, vk::Extent2D windowExtent ) 
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

vk::SurfaceFormatKHR Swapchain::chooseSurfaceFormat(std::vector<vk::SurfaceFormatKHR>& surfaceFormats) 
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

vk::PresentModeKHR Swapchain::choosePresentMode(std::vector<vk::PresentModeKHR>& presentModes) 
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