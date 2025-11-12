

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

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 shininess;
};

layout(binding = 1) buffer ShaderStorageBufferObject1 {
    Material mats[2];
} ssbo;

layout(binding = 2) uniform sampler2D texSampler1[10];

//layout(binding = 2) uniform sampler2D texSampler2;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec3 Normal;
layout(location = 4) in vec3 ID;
//layout(location = 2) flat in uint testuboval;


layout(location = 0) out vec4 outColor;


void old() {

    //ambient
    //float ambientStrength = 0.1f;
    vec3 lightamb = vec3(0.0001f); //we should uniform this in or something
    vec3 lightdiff = vec3(0.2f);
    vec3 lightspec = vec3(1.0f);

    float dist = length(ubo.lightpos - fragPos);
    float attenuation = 1.0 / (ubo.clq1.x + ubo.clq1.y * dist + 
    		    ubo.clq1.z * (dist * dist));
    //vec3 ambient = ambientStrength * lightColor;
    //vec3 ambient = ssbo.mats[ubo.matidx].ambient; //ubo.ambient; //vec3(0.0, 0.1, 0.06);
    //vec3 ambient = ubo.ambient * lightColor * ambientStrength;
    vec3 ambient = texture(texSampler1[ubo.imgstate], fragTexCoord).xyz * lightamb;

    //diffuse
    //vec3 diffcol = ssbo.mats[ubo.matidx].diffuse; //ubo.diffuse; //vec3( 0.0,	0.50980392,	0.50980392 );
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(ubo.lightpos - fragPos); //vec3(0.2f, 1.0f, 0.3f); 
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = lightdiff * diff * texture(texSampler1[ubo.imgstate], fragTexCoord).xyz;

    //float specularStrength = 40.5;
    //vec3 speccol = texture(texSampler1[ubo.visstate], fragTexCoord).rgb;
    //ssbo.mats[ubo.matidx].specular; //ubo.specular; //vec3( 0.50196078,	0.50196078,	0.50196078 );
    vec3 viewDir = normalize(ubo.viewpos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec3 specular = lightspec * spec * texture(texSampler1[ubo.visstate], fragTexCoord).xyz;

    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation; 
    vec3 result = (ambient + diffuse + specular); // * fragColor.xyz;
    //outColor = vec4(result, 1.0f);

}

vec3 CalcDirLight(vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(vec3(0.2f, 1.0f, 0.3f));
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0f);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    // combine results
    vec3 lightamb = vec3(0.001f); //we should uniform this in or something
    vec3 lightdiff = vec3(1.0f);
    vec3 lightspec = vec3(1.0f);
    vec3 ambient  = lightamb  * texture(texSampler1[ubo.imgstate], fragTexCoord).xyz;
    vec3 diffuse  = lightdiff  * diff * texture(texSampler1[ubo.imgstate], fragTexCoord).xyz;
    vec3 specular = lightspec * spec * texture(texSampler1[ubo.visstate], fragTexCoord).xyz;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(vec3 normal, vec3 FragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(ubo.lightpos - FragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    // attenuation
    float dist = length(ubo.lightpos - FragPos);
    float attenuation = 1.0 / (ubo.clq1.x + ubo.clq1.y * dist + 
    		    ubo.clq1.z * (dist * dist));
    // combine results
    //vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    //vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    //vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    vec3 lightamb = vec3(0.1f); //we should uniform this in or something
    vec3 lightdiff = vec3(1.0f);
    vec3 lightspec = vec3(1.0f);
    vec3 ambient  = lightamb  * texture(texSampler1[ubo.imgstate], fragTexCoord).xyz;
    vec3 diffuse  = lightdiff  * diff * texture(texSampler1[ubo.imgstate], fragTexCoord).xyz;
    vec3 specular = lightspec * spec * texture(texSampler1[ubo.visstate], fragTexCoord).xyz;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(vec3 spotLightPos, vec3 normal, vec3 FragPos, vec3 spotDir) {

    float phi = cos(radians(12.5f)); //inner cone
    float gamma = cos(radians(25.0f)); //outer cone
    float epsilon = phi - gamma;

    vec3 lightDir = normalize(spotLightPos - FragPos);
    float theta = dot(lightDir, -spotDir);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(lightDir, reflectDir), 0.0), 128);
    vec3 lightamb = vec3(0.5f);
    vec3 lightdiff = vec3(0.9f);
    vec3 lightspec = vec3(5.0f);
    vec3 ambient  = lightamb  * texture(texSampler1[ubo.imgstate], fragTexCoord).xyz;
    vec3 diffuse  = lightdiff  * diff * texture(texSampler1[ubo.imgstate], fragTexCoord).xyz;
    vec3 specular = lightspec * spec * texture(texSampler1[ubo.visstate], fragTexCoord).xyz;

    // attenuation
    float dist = length(spotLightPos - FragPos);
    float attenuation = 1.0 / (ubo.clq1.x + (ubo.clq1.y / 2.0f) * dist + 
    		    (ubo.clq1.z / 4.0f) * (dist * dist));
    //float theta = dot(lightDir, normalize(-light.direction)); 
    //float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - gamma) / epsilon, 0.0, 1.0);

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;


    return (ambient + diffuse + specular);


}

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(ubo.viewpos - fragPos);

    vec3 result = vec3(0.0f);
    // phase 1: Directional lighting
    //result = CalcDirLight(norm, viewDir);
    // phase 2: Point lights
    //for(int i = 0; i < NR_POINT_LIGHTS; i++)
    result += CalcPointLight(norm, fragPos, viewDir);
    // phase 3: Spot light
    result += CalcSpotLight(ubo.viewpos, norm, fragPos, ubo.camerafront);
    result += CalcSpotLight(ubo.lightpos2, norm, fragPos, vec3(0.0f,-1.0f,0.0f));
    
    outColor = vec4(result, 1.0);
}