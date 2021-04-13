#pragma once

#include "VulkanBase.hpp"
#include "Window.hpp"
// #include "Command.hpp"
// #include "Synchronous.hpp"
#include "Graphics.hpp"

#ifndef MAX_FRAME
    #define MAX_FRAME 2
#endif

class Engine
{
public:
    Engine();
    // ~Engine();
    void init();
    void draw();
    void loop();
    void destroy();

private:
    struct Synchronous
    {
        vk::Semaphore _presentSemaphore;
        vk::Semaphore _renderSemaphore;
        vk::Fence     _renderFence;
    };
    struct Command
    {
        vk::CommandPool   _cmdPool;
        vk::CommandBuffer _cmdBuffer;
    };

private:
    Window                                  __window;
    VulkanBase                              __vulkanbase;
    std::array<Command, MAX_FRAME>          __commands;
    std::array<Synchronous, MAX_FRAME>      __syncrhonouses;
    Graphics                                __graphics;
    uint32_t                                __frameNumber           = 0;
    uint32_t                                __frameInUse            = 0;
    DeletionQueue                           __delQueue;

private:
    vk::Device  __device;
};