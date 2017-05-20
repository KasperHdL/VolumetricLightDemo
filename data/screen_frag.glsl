#version 400

in vec2 uv;

uniform sampler2DMS position_texture;
uniform sampler2DMS normal_texture;
uniform sampler2DMS color_texture;

uniform vec4 camera_position;

uniform vec4 fog;
uniform float time;


out vec3 color;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(){
    vec3 position       = vec3(texelFetch(position_texture       , uv, 0));
    vec3 normal         = vec3(texelFetch(normal_texture         , uv, 0));
    vec3 albedo         = vec3(texelFetch(color_texture          , uv, 0));

    float l = length(position - vec3(camera_position));


    float i = l * fog.w;
    if(i > 1)i = 1;

    color = fog.xyz * i * (rand(uv * time));

}

