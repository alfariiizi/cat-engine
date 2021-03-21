#!/bin/zsh

VOP=$(pwd)/spv/vertex
FOP=$(pwd)/spv/fragment

_01=simpletriangle
_02=simpleTexture



# Vertex Shader
glslc -o $VOP/$_01.spv $_01.vert
glslc -o $VOP/$_02.spv $_02.vert



# Fragment Shader
glslc -o $FOP/$_01.spv $_01.frag
glslc -o $FOP/$_02.spv $_02.frag
