#version 400

layout (location = 0) in vec3 position;

uniform mat4 mvp;

out vec4 shadow_coord;

void main(){
    gl_Position = mvp * vec4(position, 1);
}

