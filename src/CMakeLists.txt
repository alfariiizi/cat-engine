
project( source_proj )

add_library( ${PROJECT_NAME}
#engine
    engine/Engine.cpp
    # engine/Engine.hpp
#window
    window/Window.cpp
    # window/Window.hpp
#base
    # base/DebugUtilsMessenger.hpp
    # base/QueueFamilyIndices.hpp
    base/vk_init.cpp
    # base/vk_init.hpp
    base/vk_utils.cpp
    # base/vk_utils.hpp
    base/VulkanBase.cpp
    # base/VulkanBase.hpp
#graphics
    # graphics/Graphics.hpp
    graphics/Graphics.cpp
    graphics/Material.cpp
    # graphics/Material.hpp
    # graphics/ShaderStruct.hpp
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC
        graphics/
        engine/
        window/
)



target_link_libraries( ${PROJECT_NAME}
    PUBLIC
        vulkan
        glfw
)


# -- Code below are very identic to external/cmake

target_link_directories( ${PROJECT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/external/glfw/src/
)

target_link_libraries( ${PROJECT_NAME}
    PUBLIC
        glfw
)

# Vookoo and VMA
target_include_directories( ${PROJECT_NAME} 
    PUBLIC
        # Vookoo/external/andyzip
        # Vookoo/external/gilgamesh
        # Vookoo/external/glfw/include
        # Vookoo/external/glm
        # Vookoo/external/shaderc
        # Vookoo/external/stb
        ${CMAKE_SOURCE_DIR}/external/Vookoo/include/
        ${CMAKE_SOURCE_DIR}/external/VulkanMemoryAllocator-Hpp/
        ${CMAKE_SOURCE_DIR}/external/stb/include/
)