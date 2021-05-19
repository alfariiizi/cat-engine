cmake_minimum_required( VERSION 3.16 )

project( graphics-handling )

# Creating library
add_library( graphics
    Graphics.cpp
    Material.cpp
    Mesh.cpp
)

# add the path to the library
target_include_directories( graphics
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries( graphics
    PUBLIC
        vk-base
        # misscell  # already include by vk-base
)

message( STATUS "Graphics has been called" )