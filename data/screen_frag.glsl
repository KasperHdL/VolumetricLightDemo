#version 400

in vec2 uv;

uniform sampler2D position_texture;
uniform sampler2D normal_texture;
uniform sampler2D color_texture;

uniform vec4 camera_position;

uniform vec4 fog;
uniform float position_rand;
uniform float time;

out vec3 color;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(){
    vec3 position       = vec3(texture(position_texture       , uv));
    vec3 normal         = vec3(texture(normal_texture         , uv));
    vec3 albedo         = vec3(texture(color_texture          , uv));


    float a = position_rand;
    position.x += (rand(uv * time * 1)-.5f) * a;
    position.y += (rand(uv * time * 2)-.5f) * a;
    position.z += (rand(uv * time * 3)-.5f) * a;

    float l = length(position - vec3(camera_position));

    float i = l * fog.w;
    if(i > 1)i = 1;

    color = fog.xyz * i;


}

