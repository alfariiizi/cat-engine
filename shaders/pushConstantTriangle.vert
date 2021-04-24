#version 450

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec3 vNormal;
layout( location = 2 ) in vec3 vColor;

layout( location = 0 ) out vec3 fragColor;

// push constant block
layout( push_constant ) uniform constants
{
    // these type must match to C++ code, otherwise GPU will read incorrectly.
    vec4 data;
    mat4 renderMatrix;
} PushConstants;

void main()
{
    gl_Position = PushConstants.renderMatrix * vec4( vPosition.x, -vPosition.y, vPosition.z, 1.0f );
    fragColor = vColor;
}