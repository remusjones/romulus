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

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 texCoord;

void main() {
    gl_Position = sceneData.proj * sceneData.view * inPushConstants.model * vec4(inPosition, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(inPushConstants.model)));
    vec3 worldNormal = normalize(normalMatrix * inNormal);

    vec3 lightDir = normalize(sceneData.position - vec3(inPushConstants.model * vec4(inPosition, 1.0)));
    float distance = length(sceneData.position - vec3(inPushConstants.model * vec4(inPosition, 1.0)));

    vec3 ambient = sceneData.ambientStrength * sceneData.color;
    float diff = max(dot(worldNormal, lightDir), 0.0);

    // Half Lambert shading
    float lambertTerm = 0.5 * (dot(worldNormal, lightDir) + 1.0);
    vec3 diffuse = lambertTerm * diff * sceneData.color * sceneData.lightIntensity /
    (1.0 + 0.1 * distance + 0.01 * (distance * distance));

    vec3 viewDir = normalize(-vec3(inPushConstants.model * vec4(inPosition, 1.0)));
    vec3 reflectDir = reflect(-lightDir, worldNormal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialProperties.shininess);
    vec3 specular = materialProperties.specularStrength * spec *
    (sceneData.color * sceneData.lightIntensity) /
    (1 + 0.1 * distance + 0.01 * (distance * distance));

    // Uses vertex color as influence
    vec3 result = (ambient + diffuse + (specular)) * (inColor * vec3(materialProperties.color));

    fragColor = result;
    texCoord = inTexCoord;

}
