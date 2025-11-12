

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObjectL2 {
    mat4 model;
    mat4 view;
    mat4 proj;
    float timeval;
    uint testval;
} ubo;
layout(binding = 1) uniform sampler2D texSampler1[2];
//layout(binding = 2) uniform sampler2D texSampler2;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
//layout(location = 2) flat in uint testuboval;


layout(location = 0) out vec4 outColor;
vec3 pallete(float t, vec3 a, vec3 b, vec3 c, vec3 d) {
    return a + b*cos( 6.28318*(c*t*d) );
}

void main() {
   
    vec2 uv = (fragTexCoord.xy * 2.0) - 1.0;
    vec2 uv0 = uv;
    vec3 finalColor = vec3(0.0f);

    for(float i = 0.0f; i < 4.0f; i++) {
        uv = fract(uv * 1.5f) - 0.5f;
        //uv -= 0.5f;
        //outColor = vec4(0.0f,0.0f,0.0f,0.5f);
        float d = length(uv) * exp(-length(uv0));

        //vec3 col = {1.0f, 2.0f, 3.0f};
        vec3 col = pallete(length(uv0) + i*0.4f + ubo.timeval,
            vec3(0.5f,0.5f,0.5f),
            vec3(0.5f,0.5f,0.5f),
            vec3(1.0f,1.0f,1.0f),
            vec3(0.263f,0.416f,0.557f)
        );
        //d -= 0.5f;
        //d += ubo.timeval;
        d = sin(d*8.0f + ubo.timeval*2.0f)/8.0f;
        d = abs(d);
    
        //d = smoothstep(0.0f,0.1f,d);
        d = pow(0.01 / d, 3.0f);
        finalColor += col * d;
        //outColor = vec4(uv,0.5f * sin(uv.x),0.9f);
        //outColor = vec4(d,d,0.0f,d);
    }
    outColor = vec4(finalColor, 0.9f);
    //} else {
        //outColor =  vec4(r,g,b,a); //texture(texSampler2, fragTexCoord); //vec4(r,g,b,a);//fragColor; //
      //  outColor =  texture(texSampler1[1], fragTexCoord); //vec4(r,g,b,a);//fragColor; //
    //}
}

