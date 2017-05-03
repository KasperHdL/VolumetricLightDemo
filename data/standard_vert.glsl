#version 400
layout(location = 0)in vec4 position;
layout(location = 1)in vec3 normal;
layout(location = 2)in vec2 uv;
layout(location = 3)in vec4 c;

out vec3 vNormal;
out vec2 vUV;
out vec3 vEyePos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;

void main(void) {
/*
    vec4 eyePos = view * model * position;
    gl_Position = projection * eyePos;
    vNormal = normal_matrix * normal;
    vUV = uv;
    vEyePos = eyePos.xyz;
    */
    gl_Position = position;
}

