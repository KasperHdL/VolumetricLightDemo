#version 400

in vec2 uv;

uniform sampler2D position_texture;
uniform sampler2D normal_texture;
uniform sampler2D color_texture;

uniform vec4 camera_position;

uniform vec4 fog;


out vec3 color;

void main(){
    vec3 position       = vec3(texture(position_texture       , uv));
    vec3 normal         = vec3(texture(normal_texture         , uv));
    vec3 albedo         = vec3(texture(color_texture          , uv));

    float l = length(position - vec3(camera_position));


    float i = l * fog.w;
    if(i > 1)i = 1;

    color = fog.rgb * i;

}

