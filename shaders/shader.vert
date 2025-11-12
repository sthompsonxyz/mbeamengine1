#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 lightpos;
    vec3 lightpos2;
    vec3 viewpos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 globallightdir;
    vec3 clq1;
    vec3 camerafront;
    uint imgstate;
    uint visstate;
    uint matidx;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 inID;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragPos;
layout(location = 3) out vec3 outNormal;
layout(location = 4) out vec3 ID;
//layout(location = 2) flat out uint testuboval;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragPos = vec3(ubo.model *  vec4(inPosition, 1.0));
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    ID = inID;
    outNormal = mat3(transpose(inverse(ubo.model))) * inNormal; 
    //outNormal = inNormal;
    //testuboval = ubo.testval;
}