cmake_minimum_required( VERSION 3.16 )

project( cat-engine )

set( CMAKE_CXX_STANDARD 17 )

add_executable( ${PROJECT_NAME}
    src/Command.cpp
    # src/Depth.cpp
    src/Engine.cpp
    src/Graphics.cpp
    # src/Image.cpp
    src/main.cpp
    src/Pipeline.cpp
    src/Renderpass.cpp
    src/Swapchain.cpp
    src/Synchronous.cpp
    src/VulkanBase.cpp
    src/Window.cpp
    src/vk_utils.cpp
    src/vk_init.cpp
    src/Material.cpp
)

target_link_libraries( ${PROJECT_NAME}
    PUBLIC
        vulkan
        glfw
)

add_subdirectory( ${CMAKE_SOURCE_DIR}/external )

target_include_directories( ${PROJECT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

# add_subdirectory( ${CMAKE_SOURCE_DIR}/shaders )