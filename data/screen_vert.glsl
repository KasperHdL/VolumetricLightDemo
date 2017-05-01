#version 150

in vec2 position;
in vec2 texcoord;
out vec2 t;

void main(){
    t = texcoord;
    gl_Position = vec4(position, 0.0, 1.0);

}

