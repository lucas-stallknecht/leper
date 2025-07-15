#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 transform;

out vec3 vCol;

void main()
{
    gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vCol = aPos.xyz;
}
