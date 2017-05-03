#version 400

in vec2 uv;

uniform sampler2D position_texture;
uniform sampler2D normal_texture;
uniform sampler2D color_texture;

uniform vec4 light_dir;

out vec3 color;

void main(){


    vec3 position = vec3(texture(position_texture, uv));
    vec3 normal   = vec3(texture(normal_texture, uv));
    vec3 nn   = (normalize(normal) - vec3(0.5,0.5,0.5)) * 2.0;
    vec3 diffuse  = vec3(texture(color_texture, uv));

    float d =  dot(normalize(nn), normalize(vec3(light_dir)));

    color = diffuse * d;
}
