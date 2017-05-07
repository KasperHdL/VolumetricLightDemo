#version 400
in vec4 p;
in vec3 n;
in vec2 t;
in vec4 c;

in vec4 shadow_coord;

uniform sampler2D shadow_map;

layout(location = 0)out vec3 position;
layout(location = 1)out vec3 normal;
layout(location = 2)out vec3 color;

void main(void){
    position = vec3(p);
    normal = normalize(n);

    float visibility = 1;

    float shadow_depth = texture(shadow_map, shadow_coord.xy).z;
    
    if(shadow_depth > 0 && shadow_depth < shadow_coord.z)
        visibility = 0.5f;

    color = visibility * c.rgb;
}
