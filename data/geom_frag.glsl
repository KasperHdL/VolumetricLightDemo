#version 400
in vec4 p;
in vec4 lp;
in vec3 n;
in vec2 t;
in vec4 c;

layout(location = 0)out vec3 position;
layout(location = 1)out vec3 normal;
layout(location = 2)out vec3 color;
layout(location = 3)out vec3 local_position;


void main(void){
    position = vec3(p);
    local_position = vec3(lp);
    normal = normalize(n);
    color = c.rgb;
}
