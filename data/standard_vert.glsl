#version 140
in vec4 position;
in vec3 normal;
in vec2 uv;
out vec3 vNormal;
out vec2 vUV;
out vec3 vEyePos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMat;

void main(void) {
    vec4 eyePos = view * model * position;
    gl_Position = projection * eyePos;
    vNormal = normalMat * normal;
    vUV = uv;
    vEyePos = eyePos.xyz;
}

