#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 lightMatrix;

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

out vec3 vNorm;
out vec3 vPos;
out vec4 vPosLightSpace;


void main()
{
    vPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(vPos, 1.0);
    vNorm = normalize(mat3(transpose(inverse(model))) * aNorm);
    vPosLightSpace = lightMatrix * vec4(vPos, 1.0);
}
