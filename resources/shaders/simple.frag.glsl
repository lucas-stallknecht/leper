#version 460 core

in vec3 vNorm;

uniform vec3 sunDir;

out vec4 FragColor;

void main()
{
    // float diffuse = dot(vNorm, sunDir);
    vec3 col = vNorm * 0.5 + 0.5;
    FragColor = vec4(col, 1.0f);
} 
