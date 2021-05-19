#pragma once

#include <functional>

#include <vulkan/vulkan.hpp>
#include "vma/vk_mem_alloc.hpp"

class Buffer
{
public:
    std::function<void ()> createBuffer( const vma::Allocator&              allocator, 
                                         const vk::BufferCreateInfo&        bufferInfo, 
                                         const vma::AllocationCreateInfo&   allocInfo )
    {
        auto tmp = allocator.createBuffer( bufferInfo, allocInfo );
        __buffer = tmp.first;
        __allocation = tmp.second;

        return [&]() { allocator.destroyBuffer( __buffer, __allocation); };
    }
    vma::Allocation& allocation() { return __allocation; };
    vk::Buffer& buffer() { return __buffer; };

private:
    vma::Allocation __allocation;
    vk::Buffer __buffer;
};

class Buffer_d  // this is buffer for descriptor set
{
public:
    Buffer_d(  ){

    }

private:
    Buffer __buffer;
    vk::DescriptorSet __set;
};
