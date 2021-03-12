#include "Depth.hpp"

#include "VulkanBase.hpp"

void Depth::create()
{
    assert( _hasInitilialized_ && _hasInitDepth_ );

    vk::ImageCreateInfo imageInfo {};
    imageInfo.setImageType( vk::ImageType::e2D );
    imageInfo.setFormat( getFormat() );
    imageInfo.setExtent( _extent_ );
    imageInfo.setMipLevels( 1 );
    imageInfo.setArrayLayers( 1 );
    imageInfo.setSamples( vk::SampleCountFlagBits::e1 );
    imageInfo.setUsage( vk::ImageUsageFlagBits::eDepthStencilAttachment );
    imageInfo.setTiling( vk::ImageTiling::eOptimal );

    vma::AllocationCreateInfo allocInfo {};
    allocInfo.setUsage( vma::MemoryUsage::eGpuOnly );
    allocInfo.setRequiredFlags( vk::MemoryPropertyFlagBits::eDeviceLocal );

    {
        auto tmp = _pAllocator_->createImage( imageInfo, allocInfo );
        _hImage_ = tmp.first;
        _allocation_ = tmp.second;
    }

    vk::ImageViewCreateInfo viewInfo {};
    viewInfo.setImage( _hImage_ );
    viewInfo.setFormat( _format_ );
    viewInfo.setSubresourceRange(
        vk::ImageSubresourceRange{
            vk::ImageAspectFlagBits::eDepth,
            0, 1,
            0, 1
        }
    );
    viewInfo.setViewType( vk::ImageViewType::e2D );
    _hView_ = _pDevice_->createImageView( viewInfo );

    _hasbeenCreated_ = true;
}

void Depth::initDepth( vk::Extent3D extent ) 
{
    _extent_ = extent;
    _hasInitDepth_ = true;
}

const vk::Format& Depth::getFormat() 
{
    return _format_;
}

const vk::Extent3D& Depth::getExtent() 
{
    assert( _hasInitDepth_ );
    return _extent_;
}
