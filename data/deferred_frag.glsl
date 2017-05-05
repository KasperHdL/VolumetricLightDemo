#version 400
layout (location = 0)in vec4 p;
layout (location = 1)in vec3 n;
layout (location = 2)in vec2 t;
layout (location = 3)in vec4 c;

in vec4 shadow_coord;

uniform sampler2D depth_texture;

layout(location = 0)out vec3 position;
layout(location = 1)out vec3 normal;
layout(location = 2)out vec3 color;

void main(void){
    position = vec3(p);
    normal = normalize(n);

    float visibility = 1;

    if(texture(shadow_texture, shadow_coord.xy).z < shadow_coord.z)
        visibility = 0.5f;

    color = visibility * c.xyz;
}
