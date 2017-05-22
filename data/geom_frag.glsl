#version 400
uniform vec4 color;

in vec4 p;
in vec3 n;
in vec2 t;
in vec4 c;

layout(location = 0)out vec3 position;
layout(location = 1)out vec3 normal;
layout(location = 2)out vec3 ocolor;
layout(location = 3)out vec3 specular;


void main(void){
    position = vec3(p);
    normal = normalize(n);
    ocolor = color.xyz;
    specular = color.aaa;
}
