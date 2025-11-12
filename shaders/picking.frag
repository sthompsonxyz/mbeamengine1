#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) buffer ShaderStorageBufferObject1 {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 IDs;
} ssbo;
 
layout(location = 0) in vec4 fragColor;
//layout(location = 1) in vec2 fragTexCoord;
layout(location = 1) in vec3 ID;
//layout(location = 2) flat in uint testuboval;


//layout(location = 0) out vec4 outColor;

layout(location = 0) out float outColor;
 
void main() {
    ssbo.IDs = ID; //vec3(0.9f,0.7f,0.4f); //ID;
   
    //only needed for debugging to draw to color attachment
    outColor = ssbo.IDs[0]; //,0.9f); //ssbo.Selected_ID;
}

