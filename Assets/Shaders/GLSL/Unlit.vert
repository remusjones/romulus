#version 450


layout(push_constant)  uniform PushConstants {
    mat4 model;
} inPushConstants;

layout(set = 0, binding = 0) uniform SceneBuffer {
    vec3 position;
    float lightIntensity;
    vec3 color;
    float ambientStrength;

    mat4 view;
    mat4 proj;
} sceneData;

layout(set = 0, binding = 1) uniform MaterialProperties{
    vec4 color;
    float specularStrength;
    float shininess;
} materialProperties;

// Vertex Information
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec4 inTangent;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 uv;

void main() {

    gl_Position = sceneData.proj * sceneData.view * inPushConstants.model * vec4(inPosition, 1.0);

    vec3 result = vec3(materialProperties.color);
    fragColor = result;

}