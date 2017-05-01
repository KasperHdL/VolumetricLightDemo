#version 150

in vec4 position;
in vec3 normal;
in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMat;

out vec4 p;
out vec3 n;
out vec2 t;

void main(){
    p = view * model * position;
    gl_Position = projection * p;
    n = normalMat * normal;
}

