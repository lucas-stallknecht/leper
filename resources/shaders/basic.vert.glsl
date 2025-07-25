#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 sunDir;

uniform mat4 model;

flat out vec3 vNorm;

void main()
{
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vNorm = normalize(mat3(transpose(inverse(model))) * aNorm);
}
