#pragma once

#include <vulkan/vulkan.hpp>

/**
 * Idk, but maybe it'll use other class such as Material, Buffer, Image, Descriptor, Mesh, etc. (each 'll be an array)
 * So, it's going to be a main class for all of that.
 */

class Graphics
{
public:
    void init( const vk::Device& device );
    void create();
    void destroy();

public:

private:

private:
    bool _hasBeenInit_      = false;
    bool _hasBeenCreated_   = false;

    /* Depend */
private:
    const vk::Device*       _pDevice_;
};