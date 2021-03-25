#pragma once

#include <GLFW/glfw3.h>

/**
 * I'm not sure why I create this class, I means the window is really simple right ?
 * But maybe in the future, it'll be Main class for other class such as Keyboard, Mouse, etc.
 */

class Window
{
public:
    void init();
    void create();
    void destroy();

public:
    int shouldClose();
    void poolEvents();

public:
    GLFWwindow*         getPWindow();

public:
    const char* _windowName = "Cat Engine";
    static constexpr uint32_t ScreenWidth   = 800U;
    static constexpr uint32_t ScreenHeight  = 600U;

private:
    bool _hasBeenInit_      = false;
    bool _hasBeenCreated_   = false;

private:
    GLFWwindow*     _pWindow_;

};