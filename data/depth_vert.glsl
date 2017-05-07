#version 400

layout (location = 0) in vec3 position;

uniform mat4 shadow_vp;
uniform mat4 model;

void main(){
    gl_Position = shadow_vp * model *  vec4(position,1);
}

