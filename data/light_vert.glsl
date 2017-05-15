#version 400

layout(location = 0)in vec3 position;

uniform mat4 mvp;

out vec3 worldPos;

void main(){
    worldPos = vec3(mvp * vec4(position, 1));
    gl_Position = mvp * vec4(position, 1);
}
