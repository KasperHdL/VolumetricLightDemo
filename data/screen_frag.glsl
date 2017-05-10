#version 400

in vec2 uv;

uniform sampler2D position_texture;
uniform sampler2D normal_texture;
uniform sampler2D color_texture;

uniform sampler2D shadow_map;

uniform vec3 light_position;
uniform vec3 light_color;
uniform mat4 light_shadow_vp;
uniform int light_shadow_index;

out vec3 color;

void main(){
    vec3 position       = vec3(texture(position_texture       , uv));
    vec3 normal         = vec3(texture(normal_texture         , uv));
    vec3 albedo         = vec3(texture(color_texture          , uv));

    vec3 diffuse = vec3(0);

    diffuse *= albedo;
    color = vec3(0);

}

