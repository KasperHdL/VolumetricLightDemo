#version 400

layout (location = 0) in vec3 position;
layout (location = 2) in vec4 tex;

out vec2 uv;


void main(){
    uv = tex.xy;
    gl_Position = vec4(position, 1.0);
}

