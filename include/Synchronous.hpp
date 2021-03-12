#pragma once

#include <vulkan/vulkan.hpp>

class Synchronous
{
public:
    enum class Type
    {
        ONLY_FENCE,
        ONLY_SEMAPHORE,
        FENCE_AND_SEMAPHORE
    };

public:
    void init( const vk::Device& device, Synchronous::Type type, vk::FenceCreateFlagBits fenceFlag = {} );
    void create();
    void destroy();

public:
    vk::Fence&      getFence();
    vk::Semaphore&  getSemaphore();

private:
    Synchronous::Type           _type_;
    vk::Fence                   _fence_;
    vk::Semaphore               _semaphore_;
    vk::FenceCreateFlagBits     _fenceFlag_;

private:
    bool _hasBeenInit_       = false;
    bool _hasBeenCreated_    = false;

private:
    const vk::Device*       _pDevice_;
};