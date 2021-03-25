
project( graphics_proj )

add_library( ${PROJECT_NAME}
#source
    Graphics.cpp
    Material.cpp
#header
    Graphics.hpp
    Material.hpp
    ShaderStruct.hpp
)

target_link_libraries( ${PROJECT_NAME}
    PUBLIC
        vulkan
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)