#version 450

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


layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 2) uniform sampler2DArray textureArray;

vec3 CalculateLightDirection(vec3 lightPosition, vec3 fragmentPosition) {
    return normalize(lightPosition - fragmentPosition);
}
vec3 CalculateFinalColor(vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    return ambientColor + diffuseColor + specularColor;
}

vec3 GetNormal(int normalTextureIndex)
{
    // Perturb normal, see http://www.thetenthplanet.de/archives/1180
    vec3 tangentNormal = texture(textureArray, vec3(inUV, normalTextureIndex)).xyz * 2.0 - 1.0;

    vec3 q1 = dFdx(inFragPos);
    vec3 q2 = dFdy(inFragPos);
    vec2 st1 = dFdx(inUV);
    vec2 st2 = dFdy(inUV);

    vec3 N = normalize(inNormal);
    vec3 T = normalize(q1 * st2.t - q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
void main() {

    int albedoTextureIndex = 0;
    int normalTextureIndex = 1;
    vec3 modelNormal = GetNormal(normalTextureIndex);

    // Spec
    vec3 lightDirection = CalculateLightDirection(sceneData.light.position, inFragPos);
    float diffuse = dot(modelNormal, lightDirection) * 0.5 + 0.5;
    vec3 viewDirection = normalize(sceneData.viewPos.xyz - inFragPos);
    vec3 reflectionDirection = reflect(lightDirection, modelNormal);
    float specular = pow(max(dot(viewDirection, reflectionDirection), 0.0), materialProperties.shininess);

    // Lighting
    vec3 ambientColor = sceneData.light.ambientStrength * materialProperties.color.rgb;
    float distanceToLight = length(sceneData.light.position - inFragPos);
    float attenuation = sceneData.light.lightIntensity / (1.0 + 0.09 * distanceToLight + 0.032 * distanceToLight *
    distanceToLight);
    vec3 diffuseColor = attenuation * materialProperties.color.rgb * sceneData.light.color * diffuse;
    vec3 specularColor = attenuation * materialProperties.specularStrength * specular * sceneData.light.color;

    vec3 finalColor = CalculateFinalColor(ambientColor, diffuseColor, specularColor);
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
        outColor = vec4(specularColor, 1.0f);
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
