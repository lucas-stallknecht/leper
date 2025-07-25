#version 460 core

in vec3 vNorm;

uniform vec3 sunDir;

uniform vec3 u_color;

out vec4 FragColor;

#define N_COLORS 4.0

void main()
{
    float diffuse = 0.5 + 0.5 * dot(vNorm, sunDir);
    float quantized = floor(diffuse * N_COLORS) / N_COLORS;
    vec3 col = u_color * quantized;
    FragColor = vec4(col, 1.0);
} 
