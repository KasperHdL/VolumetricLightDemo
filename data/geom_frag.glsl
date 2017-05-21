#version 400
uniform vec4 color;

in vec4 p;
in vec4 lp;
in vec3 n;
in vec2 t;
in vec4 c;

layout(location = 0)out vec3 position;
layout(location = 1)out vec3 normal;
layout(location = 2)out vec3 albedo;
layout(location = 3)out vec3 local_position;


void main(void){
    position = vec3(p);
    local_position = vec3(lp);
    normal = normalize(n);
    albedo = color.xyz * color.a;
}
