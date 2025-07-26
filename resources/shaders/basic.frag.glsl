#version 460 core

#define MAX_POINT_LIGHTS 3
#define N_COLORS_DIR 3.0
#define N_COLORS_POINT 5.0

in vec3 vNorm;
in vec3 vPos;

struct DirLight {
    vec3 dir;
    float intensity;
    vec3 col;
};
struct PointLight {    
    vec3 pos;
    float intensity;
    vec3 col;
    float range;
    float falloff;
};  

uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform vec3 u_color;

out vec4 FragColor;


float luma(vec3 color) {
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

vec3 calcDirLight(DirLight light, vec3 n) {
    float nDotL = max(0.0, 0.5 * 0.5 + dot(n, normalize(light.dir)));
    float quantized = round(nDotL * N_COLORS_DIR) / N_COLORS_DIR;

    return light.col * light.intensity * u_color * quantized;
}

vec3 calcPointLight(PointLight light, vec3 pos, vec3 n) {
    vec3 lightDir = normalize(light.pos - pos);

    float nDotL = max(0.0, dot(n, lightDir));
    float quantized = round(nDotL * N_COLORS_POINT) / N_COLORS_POINT;

    float distance = length(light.pos - pos);
    float attenuation = 1.0 / (1.0 + distance * distance);

    return light.col * light.intensity * u_color * attenuation * quantized;
}

void main()
{
    vec3 col = vec3(0.0);
    col += calcDirLight(dirLight, vNorm);
    for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
        col += calcPointLight(pointLights[i], vPos, vNorm);
    }

    FragColor = vec4(col, 1.0);
} 
