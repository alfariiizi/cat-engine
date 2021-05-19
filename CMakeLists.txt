cmake_minimum_required( VERSION 3.16 )

include_guard( GLOBAL )

project( cat-engine )

set( CMAKE_CXX_STANDARD 17 )

set( external-path ${CMAKE_SOURCE_DIR}/external ) # it'll be used on child cmake

add_executable( main-exec
    src/main.cpp
#allocated
    # src/allocated/AllocatedImage.cpp
#base
    # src/base/vk_init.cpp
    # src/base/vk_utils.cpp
    # src/base/VulkanBase.cpp
#descriptor
    # src/descriptor/descriptor.cpp
#engine
    # src/engine/Engine.cpp
#graphics
    # src/graphics/Graphics.cpp
    # src/graphics/Material.cpp
    # src/graphics/Mesh.cpp
#utils
    # src/utils/math.cpp
    # src/utils/utils.cpp
#window
    # src/window/Window.cpp
)

# Create misscellaneous library
add_library( misscell
#allocated
    src/allocated/AllocatedImage.cpp
#descriptor
    src/descriptor/descriptor.cpp
#window
    src/window/Window.cpp
)

# include misscellaneous directory header
target_include_directories( misscell
    PUBLIC
        src/allocated
        src/window
        src/descriptor
)

# Adding all subdirectory
add_subdirectory( ${CMAKE_SOURCE_DIR}/src/base )
add_subdirectory( ${CMAKE_SOURCE_DIR}/src/graphics )
add_subdirectory( ${CMAKE_SOURCE_DIR}/external )
add_subdirectory( ${CMAKE_SOURCE_DIR}/src/engine )

# linking graphics to engine. Because before this, the engine is link to graphics but the graphics not linked to engine
target_link_libraries( graphics
    PRIVATE
        engine
)

target_link_libraries( main-exec
    PUBLIC
        # vk-base   # already include by engine
        # graphics  # already include by engine
        engine
)

target_include_directories( main-exec
    PUBLIC
        src/allocated/
        # src/base/
        # src/engine/
        # src/graphics/
        src/window/
        # src/utils/
)
