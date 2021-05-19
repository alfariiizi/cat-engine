cmake_minimum_required( VERSION 3.16 )

project( system-engine )

add_library( engine 
    Engine.cpp
)

target_include_directories( engine
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries( engine
    PUBLIC
        # vk-base   # already include by graphics
        # misscell  # already include by graphics
        graphics
)

message( STATUS "Engine has been called" )