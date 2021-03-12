#pragma once

#include <vulkan/vulkan.hpp>
#include "Swapchain.hpp"
#include "Depth.hpp"

class Renderpass
{
public:
    /* Main member function */
    void init( const vk::Device& device, const vma::Allocator& allocator, Swapchain& swapchain, bool useDepth );
    void create();
    void destroy();

    /* Function that used in Main member function */
private:
    void createFramebuffer();

    /* Getter */
public:
    const vk::RenderPass&                   getRenderpass();
    const Depth&                            getDepth();
    const std::vector<vk::Framebuffer>&     getFramebuffer();

    /* Main member variables */
private:
    vk::RenderPass                  _hRenderpass_;
    Depth                           _depth_;
    std::vector<vk::Framebuffer>    _framebuffers_;

    /* Checker */
private:
    bool _hasBeenInitialized_    = false;
    bool _hasBeenCreated_        = false;
    bool _useDepth_              = false;

    /* Depend */
private:
    const vk::Device*       _pDevice_;
    const vma::Allocator*   _pAllocator_;
    Swapchain*        _pSwapchain_;
};