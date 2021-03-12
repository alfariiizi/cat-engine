#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.hpp>


class Image
{
public:
    void init( const vk::Device& device, const vma::Allocator& allocator );
    virtual void create();
    void destroy();
    const vk::Image& getImage();
    const vk::ImageView& getImageView();

protected:
    const vk::Device*           _pDevice_;      // for creating and destroying image view
    const vma::Allocator*       _pAllocator_;   // for creating and destroying image
    bool                        _hasbeenCreated_    =          false;  // relate with destroying things after creating.
    bool                        _hasInitilialized_  =         false;  // relate with initialized things like pDevice, etc.
    vk::Image                   _hImage_;
    vma::Allocation             _allocation_;
    vk::ImageView               _hView_;
};