#version 400

in vec2 uv;

uniform sampler2D position_texture;
uniform sampler2D normal_texture;
uniform sampler2D color_texture;

uniform vec4 camera_position;

uniform vec4 fog;
uniform float position_rand;
uniform float time;

uniform vec4 light_direction;
uniform vec4 light_color;

uniform sampler2D shadow_map;
uniform int  light_shadow_index;
uniform mat4 light_shadow_vp;

out vec3 color;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float calc_shadows_smooth(int index, vec3 position){
    vec4 frag_from_light = light_shadow_vp * vec4(position,1);

    vec3 coord = frag_from_light.xyz / frag_from_light.w;
    coord = coord * 0.5 + 0.5;
    if(coord.z > 1.0) return 1.0;
    

    float bias = 0.0005;

    float shadow = 0.0;

    vec2 texel_size = 1.0  / textureSize(shadow_map, 0);

    for(int x = -1; x <= 1; x++){
        for(int y = -1; y <= 1; y++){
            float adj_depth = texture(shadow_map, coord.xy + vec2(x,y) * texel_size).r;
            shadow +=  coord.z > adj_depth + bias ? 1.0 : 0.0;

        }
    }
    shadow /= 9;


    return shadow;

}

void main(){
    vec3 position       = vec3(texture(position_texture       , uv));
    vec3 normal         = vec3(texture(normal_texture         , uv));
    vec3 albedo         = vec3(texture(color_texture          , uv));

    float shadow = 0.0;

    if(light_shadow_index >= 0)
        shadow = calc_shadows_smooth(light_shadow_index, position);

    float d = max(dot(normal, normalize(-light_direction.xyz)), 0.0);
    vec3 diffuse = d * (light_color.rgb * light_color.a * (1 - shadow));

    float a = position_rand;
    position.x += (rand(uv * time * 1)-.5f) * a;
    position.y += (rand(uv * time * 2)-.5f) * a;
    position.z += (rand(uv * time * 3)-.5f) * a;

    float l = length(position - vec3(camera_position));

    float i = l * fog.w;
    if(i > 1)i = 1;

    color = fog.xyz * i + diffuse * albedo;


}

