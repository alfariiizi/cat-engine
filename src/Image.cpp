#include "Image.hpp"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.hpp"
#include "vk_mem_alloc.h"

void Image::init(const vk::Device& device, const vma::Allocator& allocator) 
{
    _pDevice_ = &device;
    _pAllocator_ = &allocator;
    _hasInitilialized_ = true;
}

void Image::create()
{
    assert( _hasInitilialized_ );

    /* TODO: FILL THIS CREATE METHOD */

    
    _hasbeenCreated_ = true;
}

void Image::destroy() 
{
    assert( _hasbeenCreated_ );
    _pDevice_->destroyImageView( _hView_ );
    _pAllocator_->destroyImage( _hImage_, _allocation_ );

    _hasbeenCreated_ = false;
}

const vk::Image& Image::getImage() 
{
    assert( _hasbeenCreated_ );
    return _hImage_;
}

const vk::ImageView& Image::getImageView() 
{
    assert( _hasbeenCreated_ );
    return _hView_;
}