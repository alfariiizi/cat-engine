cmake_minimum_required( VERSION 3.16 )

project( vulkan-base )

add_library( vk-base 
    VulkanBase.cpp
    vk_utils.cpp
    vk_init.cpp
    Window.cpp
)

target_link_libraries( vk-base 
    PUBLIC
        vulkan
        glfw
        # misscell
)

target_include_directories( vk-base
    PUBLIC

        # vk-base header directory
        ${CMAKE_CURRENT_LIST_DIR}

        # external directory
        ${external-path}/Vookoo/include
        ${external-path}/VulkanMemoryAllocator-Hpp
        ${external-path}/stb/include
        ${external-path}/tinyobjloader
)

message( STATUS "Vulkan Base has been called" )