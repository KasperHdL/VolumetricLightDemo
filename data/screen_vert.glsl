#version 400

layout (location = 0) in vec3 position;
layout (location = 2) in vec4 tex;

out vec2 uv;
out vec4 shadow_pos;

uniform mat4 shadow_vp;

void main(){
    uv = tex.xy;
    gl_Position = vec4(position, 1.0);
    shadow_pos = shadow_vp * vec4(position, 1.0);
}

