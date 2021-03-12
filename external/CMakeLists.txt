# add_subdirectory( Vookoo )
add_subdirectory( glfw )

target_link_directories( ${CMAKE_PROJECT_NAME}
    PUBLIC
        external/glfw/src/
)

target_link_libraries( ${CMAKE_PROJECT_NAME}
    PUBLIC
        glfw
)

# Vookoo and VMA
target_include_directories( ${CMAKE_PROJECT_NAME} 
    PUBLIC
        # Vookoo/external/andyzip
        # Vookoo/external/gilgamesh
        # Vookoo/external/glfw/include
        # Vookoo/external/glm
        # Vookoo/external/shaderc
        # Vookoo/external/stb
        Vookoo/include/
        VulkanMemoryAllocator-Hpp/
)