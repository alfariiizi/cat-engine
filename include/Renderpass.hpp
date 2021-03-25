#pragma once

#include <vulkan/vulkan.hpp>
#include <memory>

#include "vku/vku.hpp"

#include "Swapchain.hpp"
#include "DeletionQueue.hpp"

#include <vma/vk_mem_alloc.hpp>

namespace vk
{
//   template <typename Dispatch> class UniqueHandleTraits<RenderPass, Dispatch> { public: using deleter = ObjectDestroy<Device, Dispatch>; };
//   using UniqueRenderPass = UniqueHandle<RenderPass, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>;
    // using SharedRenderpass = std::shared
}

class Renderpass
{
public:
    struct DepthImage
    {
        vk::Image _image;
        vk::ImageView _imageView;
    };

public:
    Renderpass() {};
    ~Renderpass();
    // State getAllMembers();

public:
    /* Main member function */
    void init( vk::PhysicalDevice physicalDevice, const vk::Device& device, const vma::Allocator& allocator, Swapchain& swapchain, bool useDepth );
    void create();
    void destroy();

    /* Function that used in Main member function */
private:
    void createFramebuffer();

    /* Getter */
public:
    const vk::RenderPass&                   getRenderpass();
    // const Depth&                            getDepth();
    std::vector<vk::Framebuffer>            getFramebuffer();

    /* Main member variables */
private:
    // vk::UniqueRenderPass                    _uhRenderpass_;
    // Depth                                   _depth_;
    vk::RenderPass                          _renderpass_;
    DepthImage                              _depth_;
    std::vector<vk::UniqueFramebuffer>      _uFramebuffers_;

    /* Checker */
private:
    bool _hasBeenInitialized_    = false;
    bool _hasBeenCreated_        = false;
    bool _useDepth_              = false;

    /* Depend */
private:
    vk::PhysicalDevice      _physicalDevice_;
    vk::Device              _device_;
    const vma::Allocator*   _pAllocator_;
    Swapchain*              _pSwapchain_;
    DeletionQueue           _delQueue_;
};