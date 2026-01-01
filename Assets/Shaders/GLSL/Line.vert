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

    Light mLight;

    mat4 view;
    mat4 proj;

} sceneData;

layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = sceneData.proj * sceneData.view * inPushConstants.model * vec4(inPosition, 1.0f);
    fragColor = inColor;
}