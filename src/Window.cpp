#include "Window.hpp"
#include <assert.h>


void Window::init() 
{
    glfwInit();
    _hasBeenInit_ = true;
}

void Window::create() 
{
    assert( _hasBeenInit_ );

    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
    glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );
    _pWindow_ = glfwCreateWindow( ScreenWidth, ScreenHeight, "Cat Engine", nullptr, nullptr );

    _hasBeenCreated_ = true;
}

void Window::destroy() 
{
    assert( _hasBeenCreated_ );

    glfwDestroyWindow( _pWindow_ );
    glfwTerminate();
}

int Window::shouldClose() 
{
    assert( _hasBeenCreated_ );
    return glfwWindowShouldClose( _pWindow_ );
}

void Window::poolEvents() 
{
    assert( _hasBeenCreated_ );
    glfwPollEvents();
}

GLFWwindow* Window::getPWindow() 
{
    assert( _hasBeenCreated_ );
    return _pWindow_;
}
