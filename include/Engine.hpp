#pragma once

#include "VulkanBase.hpp"
#include "Window.hpp"
#include "Command.hpp"
#include "Synchronous.hpp"

#define MAX_FRAME 2

class Engine
{
public:
    Engine();
    ~Engine();
    void init();
    void loop();
    void destroy();

private:
    VulkanBase      _vulkanbase_;
    Window          _window_;
    std::array<Command, MAX_FRAME>      _commands_;
    std::array<Synchronous, MAX_FRAME>  _syncrhonouses_;

private:
    bool _hasBeenInit_      = false;
    bool _hasBeenCreated_   = false;

private:
    const vk::Device*       _pDevice_;
    const vma::Allocator*   _pAllocator_;

};