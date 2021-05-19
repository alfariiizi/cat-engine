cmake_minimum_required( VERSION 3.16 )

project( engine )

add_subdirectory( base )

add_library( engineSystem
#allocated
    allocated/AllocatedImage.cpp
#graphics
    graphics/Graphics.cpp
    graphics/Material.cpp
    graphics/Mesh.cpp
#engine
    engine/Engine.cpp
#descriptor
    descriptor/descriptor.cpp
)

target_include_directories( engineSystem
    PUBLIC
        allocated
        descriptor
        graphics
        engine
)

target_link_libraries( engineSystem
    PUBLIC
        vk-base
)