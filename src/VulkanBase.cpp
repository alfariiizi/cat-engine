#include "VulkanBase.hpp"

#include "DebugUtilsMessenger.hpp"
#include "vk_init.hpp"
#include "vk_utils.hpp"

#include <GLFW/glfw3.h>


VulkanBase::~VulkanBase() 
{
    _delQueue_.flush();
    _allocator_.destroy();
    debugutils::DestroyDebugUtilsMessengerEXT( 
        static_cast<VkInstance>( _uInstance_.get() ), static_cast<VkDebugUtilsMessengerEXT>( _debugUtilsMessenger_ ), nullptr
    );
}

void VulkanBase::init( Window& window ) 
{
    /**
     * @brief Instance & Debut Utils Messenger
     */
    {
        auto appInfo = vk::ApplicationInfo {
            "Cat", VK_MAKE_VERSION( 0, 1, 0 ),
            "Cat-Engine", VK_MAKE_VERSION( 1, 0, 0 ),
            VK_API_VERSION_1_2
        };

        /**
         * @brief Debug Utils create info
         */
        vk::DebugUtilsMessengerCreateInfoEXT debugUtilsInfo {};
        debugUtilsInfo.setMessageSeverity( 
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
        );
        debugUtilsInfo.setMessageType(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        );
        debugUtilsInfo.setPfnUserCallback( debugutils::debugUtilsMessengerCallback );

        /**
         * The Extensension
         */
        auto instanceExtensions = vk::enumerateInstanceExtensionProperties();
        auto enabledExtensions = this->instanceExtensions();
        for ( const auto& extension : enabledExtensions )
        {
            auto found = std::find_if( instanceExtensions.begin(), instanceExtensions.end(),
                            [&extension]( const vk::ExtensionProperties& ext ){ return strcmp( ext.extensionName, extension ) == 0; }
            );
            assert( found != instanceExtensions.end() );
        }
        /**
         * The Validation Layers
         */
        auto instanceLayers = vk::enumerateInstanceLayerProperties();
        auto validationLayers = this->instanceValidations();
        std::vector<const char*> enableValidationLayers;
        enableValidationLayers.reserve( validationLayers.size() );
        for( const auto& layer : validationLayers )
        {
            auto found = std::find_if( instanceLayers.begin(), instanceLayers.end(),
                                [&layer]( const vk::LayerProperties& l ){ return strcmp( l.layerName, layer ); }
            );
            assert( found != instanceLayers.end() );
            enableValidationLayers.push_back( layer );
        }

        /**
         * @brief Instance create info
         */
        vk::InstanceCreateInfo instanceInfo {};
        instanceInfo.setPNext( reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>( &debugUtilsInfo ) );
        instanceInfo.setPApplicationInfo( &appInfo );
        instanceInfo.setPEnabledLayerNames( enableValidationLayers );
        instanceInfo.setPEnabledExtensionNames( enabledExtensions );

        /**
         * @brief Creating instance and debugutils
         */
        _uInstance_ = vk::createInstanceUnique( instanceInfo );
        VkDebugUtilsMessengerEXT dbgUtils;
        debugutils::CreateDebugUtilsMessengerEXT( 
           static_cast<VkInstance>( _uInstance_.get() ), 
           reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugUtilsInfo), 
           nullptr, &dbgUtils
        );
        _debugUtilsMessenger_ = static_cast<vk::DebugUtilsMessengerEXT>( dbgUtils );
    }

    /**
     * @brief Surface
     */
    {
        _surface_ = init::createSurfce( _uInstance_.get(), window.getPWindow() );
    }
    
    {
        _physicalDevice_ = init::pickPhysicalDevice( _uInstance_.get(), _surface_ );
        _uDevice_ = init::createDeviceUnique( _physicalDevice_, _surface_ );
        {
            auto queue = utils::FindQueueFamilyIndices( _physicalDevice_, _surface_ );
            auto graphicsIndices = queue.graphicsFamily.value();
            auto presentIndices = queue.presentFamily.value();
            _graphicsQueue_ = _uDevice_->getQueue( graphicsIndices, 0 );
            _presentQueue_ = _uDevice_->getQueue( presentIndices, 0 );
        }
    }

    {
        vma::AllocatorCreateInfo allocatorInfo {};
        allocatorInfo.setInstance( _uInstance_.get() );
        allocatorInfo.setPhysicalDevice( _physicalDevice_ );
        allocatorInfo.setDevice( _uDevice_.get() );

        _allocator_ = vma::createAllocator( allocatorInfo );
    }

    {
        _swapchain_->init( _physicalDevice_, _surface_, _uDevice_.get(), vk::Extent2D{ window.ScreenWidth, window.ScreenHeight } );
        _swapchain_->create();
    }

    {
        _renderpass_->init( _physicalDevice_, _uDevice_.get(), _allocator_, _swapchain_.get()->getSwapchain(), true );
        _renderpass_->create();
    }

    _hasBeenCreated_ = true;
}

void VulkanBase::destroy() 
{
    // _renderpass_.destroy();
    // _swapchain_.destroy();
    _allocator_.destroy(); // depend on instance, physical device, and device
    // _uDevice_.get().destroy();
    // _uInstance_.get().destroySurfaceKHR( _surface_ );
    debugutils::DestroyDebugUtilsMessengerEXT( 
        static_cast<VkInstance>( _uInstance_.get() ), static_cast<VkDebugUtilsMessengerEXT>( _debugUtilsMessenger_ ), nullptr
    );
    // _uInstance_.get().destroy();
}

std::vector<const char*> VulkanBase::instanceValidations() 
{
    return { "VK_LAYER_KHRONOS_validation" };
}

std::vector<const char*> VulkanBase::instanceExtensions() 
{
     uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

const vk::PhysicalDevice& VulkanBase::getPhysicalDevice() 
{
    assert( _hasBeenCreated_ );
    return _physicalDevice_;
}

const vk::SurfaceKHR& VulkanBase::getSurface() 
{
    assert( _hasBeenCreated_ );
    return _surface_;
}

const vk::Device& VulkanBase::getDevice() 
{
    assert( _hasBeenCreated_ );
    return _uDevice_.get();
}

const vma::Allocator& VulkanBase::getAllocator() 
{
    assert( _hasBeenCreated_ );
    return _allocator_;
}

const vk::Queue& VulkanBase::getGraphicsQueue() 
{
    assert( _hasBeenCreated_ );
    return _graphicsQueue_;
}

const vk::Queue& VulkanBase::getPresentQueue() 
{
    assert( _hasBeenCreated_ );
    return _presentQueue_;
}

Swapchain* VulkanBase::getSwapchain() 
{
    assert( _hasBeenCreated_ );
    return &_swapchain_;
}

Renderpass* VulkanBase::getRenderpass() 
{
    assert( _hasBeenCreated_ );
    return &_renderpass_;
}
