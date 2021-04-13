cmake_minimum_required( VERSION 3.16 )

project( cat-engine )

set( CMAKE_CXX_STANDARD 17 )

add_executable( ${PROJECT_NAME}
    src/main.cpp
#allocated
    src/allocated/AllocatedImage.cpp
#base
    src/base/vk_init.cpp
    src/base/vk_utils.cpp
    src/base/VulkanBase.cpp
#descriptor
    src/descriptor/descriptor.cpp
#engine
    src/engine/Engine.cpp
#graphics
    src/graphics/Graphics.cpp
    src/graphics/Material.cpp
    src/graphics/Mesh.cpp
#utils
    # src/utils/math.cpp
    # src/utils/utils.cpp
#window
    src/window/Window.cpp
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC
        src/allocated/
        src/base/
        src/engine/
        src/graphics/
        src/window/
        # src/utils/
)

target_link_libraries( ${PROJECT_NAME}
    PUBLIC
        vulkan
        glfw
)

add_subdirectory( ${CMAKE_SOURCE_DIR}/external )
# add_subdirectory( ${CMAKE_SOURCE_DIR}/src )

# target_link_libraries( ${PROJECT_NAME} 
#     PUBLIC
#         source_proj
# )