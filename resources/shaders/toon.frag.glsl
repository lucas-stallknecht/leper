#version 460 core

#define MAX_POINT_LIGHTS 3
#define N_COLORS_POINT 5.0

in vec3 vNorm;
in vec3 vPos;
in vec4 vPosLightSpace;

uniform vec3 viewPos;
uniform sampler2D shadowMap;

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

float calcShadow()
{
    vec3 projCoords = vPosLightSpace.xyz / vPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; 
    float closestDepth = texture(shadowMap, projCoords.xy).r;   
    float currentDepth = projCoords.z;  

    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  

    return shadow;
}


vec3 calcDirLight(DirLight light, vec3 N, vec3 V) {
    vec3 L = normalize(light.dir);
    vec3 H = normalize(V + L);

    float nDotL = max(dot(N, L), 0.0);
    float diffuse = smoothstep(0.0, 0.01, nDotL);

    float NdotH = max(dot(N, H), 0.0);
    float specular = pow(NdotH * diffuse, 64.0);
    specular = smoothstep(0.39, 0.4, specular);

    float rimDot = 1.0 - max(dot(N, V), 0.0);
    float rimIntensity = rimDot * pow(nDotL, 0.1);
    rimIntensity = smoothstep(0.746, 0.766, rimIntensity);

    // Compose final color
    vec3 diffuseCol = light.col * diffuse * light.intensity;
    vec3 specularCol = light.col * specular * light.intensity;
    vec3 rimCol = light.col * vec3(rimIntensity);

    float shadow = calcShadow();

    return ((diffuseCol + specularCol + rimCol) * (1.0 - shadow)) * u_color;
}

vec3 calcPointLight(PointLight light, vec3 pos, vec3 N) {
    vec3 L = normalize(light.pos - pos);

    float nDotL = max(0.0, dot(N, L));
    // float quantized = round(nDotL * N_COLORS_POINT) / N_COLORS_POINT;

    float distance = length(light.pos - pos);
    float attenuation = 1.0 / (1.0 + distance * distance);

    return light.col * light.intensity * u_color * attenuation * nDotL;
}

void main()
{
    vec3 viewDir = normalize(viewPos - vPos);

    vec3 col = vec3(0.0);
    col += 0.3 * u_color;
    col += calcDirLight(dirLight, vNorm, viewDir);
    // for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
    //     col += calcPointLight(pointLights[i], vPos, vNorm);
    // }

    FragColor = vec4(col, 1.0);
} 
