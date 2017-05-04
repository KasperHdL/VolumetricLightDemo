#version 400

in vec2 uv;

uniform sampler2D position_texture;
uniform sampler2D normal_texture;
uniform sampler2D color_texture;

#define MAX_LIGHTS 10
uniform int num_lights;
uniform struct Light{
    vec4 position;
    vec4 color;
    vec4 cone;
} lights[MAX_LIGHTS];

out vec3 color;

void main(){


    vec3 position = vec3(texture(position_texture, uv));
    vec3 normal   = vec3(texture(normal_texture, uv));
    vec3 nn   = (normal - vec3(0.5f)) * 2.0f;
    vec3 albedo  = vec3(texture(color_texture, uv));

    vec3 diffuse = vec3(0);

    float d = 0;
    for(int i = 0; i < num_lights; i++){
        d = dot(nn, normalize(lights[i].position.xyz));
        diffuse += max(d, 0.0) * (lights[i].color.rgb * lights[i].color.a);
    }
    color = diffuse * albedo;
}
