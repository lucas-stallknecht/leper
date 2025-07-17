#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 transform;
uniform vec3 sunDir;

flat out vec3 vNorm;

void main()
{
    gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vNorm = (transform * vec4(aNorm, 0.0)).xyz;
}
