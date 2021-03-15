#!/bin/zsh

VOP=$(pwd)/spv/vertex
FOP=$(pwd)/spv/fragment

_01=simpletriangle



# Vertex Shader
glslc -o $VOP/$_01.spv $_01.vert



# Fragment Shader
glslc -o $FOP/$_01.spv $_01.frag
