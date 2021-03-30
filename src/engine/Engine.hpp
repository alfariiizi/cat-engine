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
    ~Engine();
    void init();
    void draw();
    void loop();
    void destroy();

private:
    struct Synchronous
    {
        vk::UniqueSemaphore _presentSemaphore;
        vk::UniqueSemaphore _renderSemaphore;
        vk::UniqueFence     _renderFence;
    };
    struct Command
    {
        vk::UniqueCommandPool   _pCmdPool;
        vk::UniqueCommandBuffer _pCmdBuffer;
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