#version 450 core

layout(push_constant) uniform PushConstants {
    mat4 model;
} inPushConstants;


struct Light{
    vec3 position;
    float lightIntensity;
    vec3 color;
    float ambientStrength;
};

layout(set = 0, binding = 0) uniform SceneBuffer {
    Light light;
    mat4 view;
    mat4 viewproj;
    vec4 viewPos;
} sceneData;

layout(set = 0, binding = 1) uniform MaterialProperties {
    vec4 color;
    float specularStrength;
    float shininess;
    float DebugOutState;
    float _pad;
} materialProperties;


layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inFragPos;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec4 inTangent;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 2) uniform sampler2DArray textureArray;


vec3 CalculateViewSpaceNormal(vec3 normal, mat4 cameraView) {
    mat3 normalMatrix = mat3(cameraView);
    return normalize(normalMatrix * normal);
}

vec3 CalculateLightDirection(vec3 lightPosition, vec3 fragmentPosition) {
    return (normalize(lightPosition - fragmentPosition));
}

float CalculateDiffuse(vec3 viewSpaceNormal, vec3 lightDirection) {
    return max(dot(viewSpaceNormal, lightDirection), 0.0);
}

vec3 CalculateFinalColor(vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    return ambientColor + diffuseColor + specularColor;
}

vec3 Uncharted2Tonemap(vec3 color) {
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
}
vec3 TonemapColor(vec3 color) {
    const float exposure = 4.5f;
    const float gamma = 4.2f;

    color = Uncharted2Tonemap(color * exposure);
    color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));
    color = pow(color, vec3(1.0f /gamma));
    return color;
}
vec3 GetNormal(int normalTextureIndex)
{
    vec3 tangentNormal = texture(textureArray, vec3(inUV, normalTextureIndex)).xyz * 2.0 - 1.0;

    vec3 N = normalize(inNormal);
    vec3 T = normalize(inTangent.xyz);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangentNormal);
}

void main() {

    int albedoTextureIndex = 0;
    int normalTextureIndex = 1;
    vec3 modelNormal = GetNormal(normalTextureIndex);

    vec3 lightDirection = CalculateLightDirection(sceneData.light.position, inFragPos);

    float diffuse = max(dot(modelNormal, lightDirection), 0.0); // TODO: a little wonky?
    vec3 diffuseColor = sceneData.light.color * diffuse;

    vec3 viewDirection = normalize(sceneData.viewPos.xyz - inFragPos);
    vec3 reflectionDirection = reflect(lightDirection, modelNormal);

    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), materialProperties.shininess);
    vec3 specular = materialProperties.specularStrength * spec * sceneData.light.color;

    vec3 ambientColor = sceneData.light.ambientStrength * materialProperties.color.rgb;

    float distanceToLight = length(sceneData.light.position - inFragPos);
    float attenuation = sceneData.light.lightIntensity / (1.0 + 0.09 * distanceToLight + 0.032 * distanceToLight *
    distanceToLight);


    vec3 finalColor = CalculateFinalColor(ambientColor, diffuseColor * attenuation, specular * attenuation);

    vec3 texColor = texture(textureArray, vec3(inUV, albedoTextureIndex)).xyz;
    finalColor *= texColor;


    int debugSwitchInput = int(round(materialProperties.DebugOutState));

    switch(debugSwitchInput)
    {
        case 1:
            outColor = vec4(modelNormal, 1.0f);
            break;
        case 2:
            outColor = vec4(lightDirection, 1.0f);
            break;
        case 3:
            outColor = vec4(viewDirection, 1.0f);
            break;
        case 4:
            outColor = vec4(reflectionDirection, 1.0f);
            break;
        case 5:
            outColor = vec4(ambientColor, 1.0f);
            break;
        case 6:
            outColor = vec4(diffuseColor, 1.0f);
            break;
        case 7:
            outColor = vec4(specular, 1.0f);
            break;
        case 8:
            outColor = vec4(texColor, 1.0f);
            break;
        case 9:
            outColor = vec4(inUV, 0.0f, 1.0f);
            break;
        default:
            outColor = vec4(finalColor, 1.0f);
            break;
    }
}
