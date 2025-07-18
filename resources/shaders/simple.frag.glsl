#version 460 core

in vec3 vNorm;

uniform vec3 sunDir;

out vec4 FragColor;

#define N_COLORS 5.0

void main()
{
    float diffuse = 0.5 + 0.5 * dot(vNorm, sunDir);
    float quantized = round(diffuse * N_COLORS) / N_COLORS;
    vec3 col = vec3(quantized);
    FragColor = vec4(col, 1.0);
} 
