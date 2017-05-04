#version 400
in vec4 p;
in vec3 n;
in vec2 t;
in vec4 e;
in vec4 c;

layout(location = 0)out vec3 position;
layout(location = 1)out vec3 normal;
layout(location = 2)out vec3 color;

void main(void){
    position = vec3(p);
    normal = (normalize(n) + vec3(1,1,1)) * 0.5f;
    color = vec3(c);
}
