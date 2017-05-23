#version 400

in vec2 uv;

uniform sampler2D position_texture;
uniform sampler2D normal_texture;
uniform sampler2D color_texture;
uniform sampler2D screen_texture;
uniform sampler2D specular_texture;
uniform sampler2D shadow_map;

uniform vec4 camera_position;

uniform vec4 fog;
uniform float position_rand;
uniform float time;
uniform float kernel[9];

out vec3 result;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(){
    vec3 position  = vec3(texture(position_texture , uv));
    vec3 normal    = vec3(texture(normal_texture   , uv));
    vec3 color     = vec3(texture(color_texture    , uv));
    vec3 screen    = vec3(texture(screen_texture   , uv));
    float specular = texture(specular_texture      , uv).r;
    float shadow   = texture(shadow_map      , uv).r;


    //dithering
    float a = position_rand;
    position.x += (rand(uv * time * 1)-.5f) * a;
    position.y += (rand(uv * time * 2)-.5f) * a;
    position.z += (rand(uv * time * 3)-.5f) * a;

    //fog
    float l = length(position - vec3(camera_position));

    float i = l * fog.w;
    if(i > 1)i = 1;

    //blur
    vec3 blur = vec3(0);
    float div = 0;

    vec2 texel_size = 1.0  / textureSize(screen_texture, 0);
    for(int x = -1; x <= 1; x++){
        for(int y = -1; y <= 1; y++){
            float k = kernel[(x+1) + (y+1) * 3];
            blur += texture(screen_texture, uv + vec2(x,y) * texel_size).xyz * k;
            div += abs(k);

        }
    }
    blur /= div;
    

    //Combine
    result = blur + fog.xyz * i;
}

