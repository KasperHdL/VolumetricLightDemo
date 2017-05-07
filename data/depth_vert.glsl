#version 400

layout (location = 0) in vec3 position;

uniform mat4 shadow_mvp;
uniform mat4 model;

void main(){
    gl_Position = shadow_mvp * model * vec4(position, 1);
}

