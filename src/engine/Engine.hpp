#pragma once

#include "VulkanBase.hpp"
#include "Window.hpp"
#include "Command.hpp"
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
        vk::Semaphore       _presentSemaphore;
        vk::Semaphore       _renderSemaphore;
        vk::Fence           _renderFence;
    };

private:
    VulkanBase                              _vulkanbase_;
    Window                                  _window_;
    std::array<Command, MAX_FRAME>          _commands_;
    std::array<Synchronous, MAX_FRAME>      _syncrhonouses_;
    Graphics                                _graphics_;
    uint32_t                                _frameNumber_           = 0;
    uint32_t                                _frameInUse_            = 0;

private:
    bool _hasBeenInit_      = false;
    bool _hasBeenCreated_   = false;

private:
    const vk::Device*       _pDevice_;
    const vma::Allocator*   _pAllocator_;

};