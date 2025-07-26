#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 projection;
uniform mat4 view;

struct Light {
    vec3 sunDir;
    vec3 sunColor;

    vec3 pointPos;
    float constant;
    vec3 pointColor;
    float linear;
    float quadratic;
};
uniform Light light;

uniform mat4 model;

flat out vec3 vNorm;
flat out vec3 vPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vPos = vec3(model * vec4(aPos, 1.0));
    vNorm = normalize(mat3(transpose(inverse(model))) * aNorm);
}
