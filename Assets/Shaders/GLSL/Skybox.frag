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

layout(location = 0) in vec3 fragUVW;
layout(location = 0) out vec4 outColor;
layout(set = 0, binding = 1) uniform samplerCube samplerCubeMap;

void main() {

    outColor = texture(samplerCubeMap, vec3(fragUVW.x, fragUVW.y, fragUVW.z));
}
