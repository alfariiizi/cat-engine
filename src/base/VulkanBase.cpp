#include "VulkanBase.hpp"

#include "DebugUtilsMessenger.hpp"
#include "vk_init.hpp"
#include "vk_utils.hpp"

#include <GLFW/glfw3.h>

#include "vku/vku.hpp"
#include "utils.hpp"


VulkanBase::VulkanBase(Window& window) 
{
    // Instance & Debut Utils Messenger
    { 
        auto appInfo = vk::ApplicationInfo {
            "Cat", VK_MAKE_VERSION( 0, 1, 0 ),
            "Cat-Engine", VK_MAKE_VERSION( 1, 0, 0 ),
            VK_API_VERSION_1_2
        };

        /// Debug Utils create info
        vk::DebugUtilsMessengerCreateInfoEXT debugUtilsInfo {};
        debugUtilsInfo.setMessageSeverity( 
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
        );
        debugUtilsInfo.setMessageType(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        );
        debugUtilsInfo.setPfnUserCallback( debugutils::debugUtilsMessengerCallback );

        /// The Extensension
        auto instanceExtensions = vk::enumerateInstanceExtensionProperties();
        auto enabledExtensions = this->instanceExtensions();
        for ( const auto& extension : enabledExtensions )
        {
            auto found = std::find_if( instanceExtensions.begin(), instanceExtensions.end(),
                            [&extension]( const vk::ExtensionProperties& ext ){ return strcmp( ext.extensionName, extension ) == 0; }
            );
            assert( found != instanceExtensions.end() );
        }

        /// The Validation Layers
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

        /// Instance create info
        vk::InstanceCreateInfo instanceInfo {};
        instanceInfo.setPNext( reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>( &debugUtilsInfo ) );
        instanceInfo.setPApplicationInfo( &appInfo );
        instanceInfo.setPEnabledLayerNames( enableValidationLayers );
        instanceInfo.setPEnabledExtensionNames( enabledExtensions );

        /// Creating instance and debugutils
        __pInstance = vk::createInstanceUnique( instanceInfo );
        VkDebugUtilsMessengerEXT dbgUtils;
        debugutils::CreateDebugUtilsMessengerEXT( 
           static_cast<VkInstance>( __pInstance.get() ), 
           reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugUtilsInfo), 
           nullptr, &dbgUtils
        );
        __debugUtilsMessenger = static_cast<vk::DebugUtilsMessengerEXT>( dbgUtils );
        __delQueue.pushFunction( [i=__pInstance.get(), d=__debugUtilsMessenger, f=debugutils::DestroyDebugUtilsMessengerEXT](){
            f( static_cast<VkInstance>( i ), static_cast<VkDebugUtilsMessengerEXT>( d ), nullptr );
        });
    }

    { /// Allocator
        vma::AllocatorCreateInfo allocatorInfo {};
        allocatorInfo.setInstance( __pInstance.get() );
        allocatorInfo.setPhysicalDevice( __physicalDevice );
        allocatorInfo.setDevice( __pDevice.get() );
        allocatorInfo.setVulkanApiVersion( VK_API_VERSION_1_1 );

        __allocator = vma::createAllocator( allocatorInfo );
        __delQueue.pushFunction( [&](){
            __allocator.destroy();
        });
    }

    { /// Surface
        __surface = init::createSurfce( __pInstance.get(), window.getPWindow() );
        __delQueue.pushFunction([&](){
            __pInstance->destroySurfaceKHR( __surface );
        });
    }
    
    { /// Device
        __physicalDevice = init::pickPhysicalDevice( __pInstance.get(), __surface );
        __pDevice = init::createDevice( __physicalDevice, __surface );
        {
            auto queue = utils::FindQueueFamilyIndices( __physicalDevice, __surface );
            auto graphicsIndices = queue.graphicsFamily.value();
            auto presentIndices = queue.presentFamily.value();
            __graphicsQueue = __pDevice->getQueue( graphicsIndices, 0 );
            __presentQueue = __pDevice->getQueue( presentIndices, 0 );
        }
    }

    { /// Swapchain
        __swapchain = init::createSwapchainRaw( __physicalDevice, __pDevice.get(), __surface, { Window::ScreenWidth, Window::ScreenHeight} );
        __delQueue.pushFunction([&](){
            __pDevice->destroySwapchainKHR( __swapchain );
        });

        __scFormat = utils::chooseSurfaceFormat( __physicalDevice.getSurfaceFormatsKHR( __surface ) ).format;

        // if this retrieve of image view has an error, then do generate yourself
        init::swapchainImageAndImageViews( __pDevice.get(), __swapchain, __scFormat, __scImages, __scImageViews );
        __delQueue.pushFunction([&](){
            for( auto& iv : __scImageViews )
                __pDevice->destroyImageView( iv );
        });
    }

    { /// Renderpass
        __prpDepth = vku::DepthStencilImage{ __pDevice.get(), __physicalDevice.getMemoryProperties(), Window::ScreenWidth, Window::ScreenHeight };
        __delQueue.pushFunction( [&](){
            auto image = std::move( __prpDepth.image() );
            auto imageView = std::move( __prpDepth.imageView() );
            __pDevice->destroyImageView( imageView );
            __pDevice->destroyImage( image );
        });

        vk::AttachmentDescription colorAttachment {};
        //the attachment will have the format needed by the swapchain
        colorAttachment.setFormat           ( __scFormat );
        //1 sample, we won't be doing MSAA
        colorAttachment.setSamples          ( vk::SampleCountFlagBits::e1 );
        // we Clear when this attachment is loaded
        colorAttachment.setLoadOp           ( vk::AttachmentLoadOp::eClear );
        // we keep the attachment stored when the renderpass ends
        colorAttachment.setStoreOp          ( vk::AttachmentStoreOp::eStore );
        //we don't care about stencil
        colorAttachment.setStencilLoadOp    ( vk::AttachmentLoadOp::eDontCare );
        colorAttachment.setStencilStoreOp   ( vk::AttachmentStoreOp::eDontCare );
        //we don't know or care about the starting layout of the attachment
        colorAttachment.setInitialLayout    ( vk::ImageLayout::eUndefined );
        //after the renderpass ends, the image has to be on a layout ready for display
        colorAttachment.setFinalLayout      ( vk::ImageLayout::ePresentSrcKHR );

        /**
         * @brief COLOR Attachment Reference, this is needed for creating Subpass
         */
        vk::AttachmentReference colorAttachmentRef {};
        //attachment number will index into the pAttachments array in the parent renderpass itself
        colorAttachmentRef.setAttachment( 0 );
        // this layout 'll be use during subpass that use this attachment ref
        colorAttachmentRef.setLayout( vk::ImageLayout::eColorAttachmentOptimal );

        /**
         * @brief the renderpass will use this DEPTH attachment.
         */
        vk::AttachmentDescription depthAttachment {};
        depthAttachment.setFormat( __prpDepth.format() );
        depthAttachment.setSamples( vk::SampleCountFlagBits::e1 );  // we won't use fancy sample for right now
        depthAttachment.setLoadOp( vk::AttachmentLoadOp::eClear );
        depthAttachment.setStoreOp( vk::AttachmentStoreOp::eStore );
        depthAttachment.setStencilLoadOp( vk::AttachmentLoadOp::eClear );
        depthAttachment.setStencilStoreOp( vk::AttachmentStoreOp::eDontCare );
        depthAttachment.setInitialLayout( vk::ImageLayout::eUndefined );
        depthAttachment.setFinalLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal );

        /**
         * @brief DEPTH Attachment Reference, this is needed for creating Subpass
         */
        vk::AttachmentReference depthAttachmentRef {};
        depthAttachmentRef.setAttachment( 1 );
        depthAttachmentRef.setLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal );


        /**
         * @brief Subpass and array of attachment
         */
        std::vector<vk::AttachmentDescription> attachDescs;
        vk::SubpassDescription subpass {};
        // this bind point could be graphics, compute, or maybe ray tracing
        subpass.setPipelineBindPoint( vk::PipelineBindPoint::eGraphics );
        attachDescs.push_back( colorAttachment );
        subpass.setColorAttachments( colorAttachmentRef );

        subpass.setPDepthStencilAttachment( &depthAttachmentRef );
        attachDescs.push_back( depthAttachment );

        vk::RenderPassCreateInfo renderpassInfo {};
        renderpassInfo.setAttachments( attachDescs );
        renderpassInfo.setSubpasses( subpass );

        __renderpass = __pDevice->createRenderPass( renderpassInfo );
        __delQueue.pushFunction([&](){
            __pDevice->destroyRenderPass( __renderpass );
        });
    }
    { /// Framebuffers
        __rpFramebuffers.reserve( __scImages.size() );

        for( const auto& view : __scImageViews )
        {
            std::vector<vk::ImageView> attachments = { view, __prpDepth.imageView() };

            vk::FramebufferCreateInfo fbInfo {};
            fbInfo.setAttachments( attachments );
            fbInfo.setRenderPass( __renderpass );
            fbInfo.setWidth( Window::ScreenWidth );
            fbInfo.setHeight( Window::ScreenHeight );
            fbInfo.setLayers( 1 );

            __rpFramebuffers.emplace_back( __pDevice->createFramebufferUnique( fbInfo ) );
        }
        __delQueue.pushFunction([d=__pDevice.get(), fbs=__rpFramebuffers](){
            for( auto& fb : fbs )
                d.destroyFramebuffer(fb);
        });
    }
}

VulkanBase::~VulkanBase() 
{
    __delQueue.flush();
    // __pInstance->destroySurfaceKHR( __surface );
    // debugutils::DestroyDebugUtilsMessengerEXT( 
    //     static_cast<VkInstance>( __pInstance.get() ), static_cast<VkDebugUtilsMessengerEXT>( __debugUtilsMessenger ), nullptr
    // );
}

// void VulkanBase::init( Window& window ) 
// {
//     // Instance & Debut Utils Messenger
//     { 
//         auto appInfo = vk::ApplicationInfo {
//             "Cat", VK_MAKE_VERSION( 0, 1, 0 ),
//             "Cat-Engine", VK_MAKE_VERSION( 1, 0, 0 ),
//             VK_API_VERSION_1_2
//         };

//         /// Debug Utils create info
//         vk::DebugUtilsMessengerCreateInfoEXT debugUtilsInfo {};
//         debugUtilsInfo.setMessageSeverity( 
//             vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
//         );
//         debugUtilsInfo.setMessageType(
//             vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
//         );
//         debugUtilsInfo.setPfnUserCallback( debugutils::debugUtilsMessengerCallback );

//         /// The Extensension
//         auto instanceExtensions = vk::enumerateInstanceExtensionProperties();
//         auto enabledExtensions = this->instanceExtensions();
//         for ( const auto& extension : enabledExtensions )
//         {
//             auto found = std::find_if( instanceExtensions.begin(), instanceExtensions.end(),
//                             [&extension]( const vk::ExtensionProperties& ext ){ return strcmp( ext.extensionName, extension ) == 0; }
//             );
//             assert( found != instanceExtensions.end() );
//         }

//         /// The Validation Layers
//         auto instanceLayers = vk::enumerateInstanceLayerProperties();
//         auto validationLayers = this->instanceValidations();
//         std::vector<const char*> enableValidationLayers;
//         enableValidationLayers.reserve( validationLayers.size() );
//         for( const auto& layer : validationLayers )
//         {
//             auto found = std::find_if( instanceLayers.begin(), instanceLayers.end(),
//                                 [&layer]( const vk::LayerProperties& l ){ return strcmp( l.layerName, layer ); }
//             );
//             assert( found != instanceLayers.end() );
//             enableValidationLayers.push_back( layer );
//         }

//         /// Instance create info
//         vk::InstanceCreateInfo instanceInfo {};
//         instanceInfo.setPNext( reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>( &debugUtilsInfo ) );
//         instanceInfo.setPApplicationInfo( &appInfo );
//         instanceInfo.setPEnabledLayerNames( enableValidationLayers );
//         instanceInfo.setPEnabledExtensionNames( enabledExtensions );

//         /// Creating instance and debugutils
//         __pInstance = vk::createInstanceUnique( instanceInfo );
//         VkDebugUtilsMessengerEXT dbgUtils;
//         debugutils::CreateDebugUtilsMessengerEXT( 
//            static_cast<VkInstance>( __pInstance.get() ), 
//            reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugUtilsInfo), 
//            nullptr, &dbgUtils
//         );
//         __debugUtilsMessenger = static_cast<vk::DebugUtilsMessengerEXT>( dbgUtils );
//     }

//     { /// Surface
//         __surface = init::createSurfce( __pInstance.get(), window.getPWindow() );
//     }
    
//     { /// Device
//         __physicalDevice = init::pickPhysicalDevice( __pInstance.get(), __surface );
//         __pDevice = init::createDevice( __physicalDevice, __surface );
//         {
//             auto queue = utils::FindQueueFamilyIndices( __physicalDevice, __surface );
//             auto graphicsIndices = queue.graphicsFamily.value();
//             auto presentIndices = queue.presentFamily.value();
//             __graphicsQueue = __pDevice->getQueue( graphicsIndices, 0 );
//             __presentQueue = __pDevice->getQueue( presentIndices, 0 );
//         }
//     }

//     { /// Swapchain
//         __swapchain = init::createSwapchain( __physicalDevice, __pDevice.get(), __surface, { Window::ScreenWidth, Window::ScreenHeight} );
//         __scFormat = utils::chooseSurfaceFormat( __physicalDevice.getSurfaceFormatsKHR( __surface ) ).format;

//         // if this retrieve of image view has an error, then do generate yourself
//         init::swapchainImageAndImageViews( __pDevice.get(), __swapchain, __scFormat, __scImages, __scImageViews );
//     }

//     {
//         __prpDepth = vku::DepthStencilImage{ __pDevice.get(), __physicalDevice.getMemoryProperties(), Window::ScreenWidth, Window::ScreenHeight };

//         vk::AttachmentDescription colorAttachment {};
//         //the attachment will have the format needed by the swapchain
//         colorAttachment.setFormat           ( __scFormat );
//         //1 sample, we won't be doing MSAA
//         colorAttachment.setSamples          ( vk::SampleCountFlagBits::e1 );
//         // we Clear when this attachment is loaded
//         colorAttachment.setLoadOp           ( vk::AttachmentLoadOp::eClear );
//         // we keep the attachment stored when the renderpass ends
//         colorAttachment.setStoreOp          ( vk::AttachmentStoreOp::eStore );
//         //we don't care about stencil
//         colorAttachment.setStencilLoadOp    ( vk::AttachmentLoadOp::eDontCare );
//         colorAttachment.setStencilStoreOp   ( vk::AttachmentStoreOp::eDontCare );
//         //we don't know or care about the starting layout of the attachment
//         colorAttachment.setInitialLayout    ( vk::ImageLayout::eUndefined );
//         //after the renderpass ends, the image has to be on a layout ready for display
//         colorAttachment.setFinalLayout      ( vk::ImageLayout::ePresentSrcKHR );

//         /**
//          * @brief COLOR Attachment Reference, this is needed for creating Subpass
//          */
//         vk::AttachmentReference colorAttachmentRef {};
//         //attachment number will index into the pAttachments array in the parent renderpass itself
//         colorAttachmentRef.setAttachment( 0 );
//         // this layout 'll be use during subpass that use this attachment ref
//         colorAttachmentRef.setLayout( vk::ImageLayout::eColorAttachmentOptimal );

//         /**
//          * @brief the renderpass will use this DEPTH attachment.
//          */
//         vk::AttachmentDescription depthAttachment {};
//         depthAttachment.setFormat( __prpDepth.format() );
//         depthAttachment.setSamples( vk::SampleCountFlagBits::e1 );  // we won't use fancy sample for right now
//         depthAttachment.setLoadOp( vk::AttachmentLoadOp::eClear );
//         depthAttachment.setStoreOp( vk::AttachmentStoreOp::eStore );
//         depthAttachment.setStencilLoadOp( vk::AttachmentLoadOp::eClear );
//         depthAttachment.setStencilStoreOp( vk::AttachmentStoreOp::eDontCare );
//         depthAttachment.setInitialLayout( vk::ImageLayout::eUndefined );
//         depthAttachment.setFinalLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal );

//         /**
//          * @brief DEPTH Attachment Reference, this is needed for creating Subpass
//          */
//         vk::AttachmentReference depthAttachmentRef {};
//         depthAttachmentRef.setAttachment( 1 );
//         depthAttachmentRef.setLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal );


//         /**
//          * @brief Subpass and array of attachment
//          */
//         std::vector<vk::AttachmentDescription> attachDescs;
//         vk::SubpassDescription subpass {};
//         // this bind point could be graphics, compute, or maybe ray tracing
//         subpass.setPipelineBindPoint( vk::PipelineBindPoint::eGraphics );
//         attachDescs.push_back( colorAttachment );
//         subpass.setColorAttachments( colorAttachmentRef );

//         subpass.setPDepthStencilAttachment( &depthAttachmentRef );
//         attachDescs.push_back( depthAttachment );

//         vk::RenderPassCreateInfo renderpassInfo {};
//         renderpassInfo.setAttachments( attachDescs );
//         renderpassInfo.setSubpasses( subpass );

//         __renderpass = __pDevice->createRenderPassUnique( renderpassInfo );
//     }

//     // _hasBeenCreated_ = true;
// }

// void VulkanBase::destroy() 
// {
//     __pInstance->destroySurfaceKHR( __surface );
//     debugutils::DestroyDebugUtilsMessengerEXT( 
//         static_cast<VkInstance>( __pInstance.get() ), static_cast<VkDebugUtilsMessengerEXT>( __debugUtilsMessenger ), nullptr
//     );
// }

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

// const vk::PhysicalDevice& VulkanBase::getPhysicalDevice() 
// {
//     assert( _hasBeenCreated_ );
//     return __physicalDevice;
// }

// const vk::SurfaceKHR& VulkanBase::getSurface() 
// {
//     assert( _hasBeenCreated_ );
//     return __surface;
// }

// const vk::Device& VulkanBase::getDevice() 
// {
//     assert( _hasBeenCreated_ );
//     return __pDevice.get();
// }

// const vk::Queue& VulkanBase::getGraphicsQueue() 
// {
//     assert( _hasBeenCreated_ );
//     return __graphicsQueue;
// }

// const vk::Queue& VulkanBase::getPresentQueue() 
// {
//     assert( _hasBeenCreated_ );
//     return __presentQueue;
// }