#version 400

in vec4 position;
in vec3 normal;
in vec2 texcoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normal_matrix;

out vec4 p;
out vec3 n;
out vec2 t;

void main(){
    t = texcoord;

    p = view * model * position;
    n = normal_matrix * normal;

    gl_Position = projection * p;
}

