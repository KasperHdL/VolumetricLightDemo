#version 400

in vec2 uv;
in vec4 shadow_pos;

uniform vec4 camera_position;

uniform sampler2D position_texture;
uniform sampler2D normal_texture;
uniform sampler2D color_texture;

uniform sampler2D shadow_map;

#define MAX_LIGHTS 10
uniform int num_lights;
uniform struct Light{
    vec4 position;
    vec4 color;
    vec4 attenuation;
    vec4 cone;
} lights[MAX_LIGHTS];

out vec3 color;

float shadow_calc(vec4 frag_light, vec3 position, vec3 light_dir, vec3 normal){
    vec3 coord = frag_light.xyz / frag_light.w;

    coord = coord * 0.5 + 0.5;

    float light_depth = texture(shadow_map, uv).r;
    return light_depth;

    float depth = coord.z;
    if(light_depth > 1.0) return 0.0;

    float bias = max(0.05 * (1.0 - dot(normal, normalize(-light_dir))), 0.005);

    float shadow = 0;

    shadow = depth > light_depth ? 1.0 : 0.0;

    return depth;

}

void main(){
    vec3 position = vec3(texture(position_texture, uv));
    vec3 normal   = vec3(texture(normal_texture, uv));
    vec3 albedo   = vec3(texture(color_texture, uv));

    vec3 diffuse = vec3(0);

    vec3 view_direction = camera_position.xyz - position;

    float att = 1;

    float dist;
    float cut_off;
    vec3 light_direction;
    for(int i = 0; i < 1; i++){
        cut_off = 0;

        if(lights[i].position.w == 0){
            //directional
            light_direction = -lights[i].position.xyz;
            att = 1;
        }else if(lights[i].position.w == 1){
            //point
            light_direction = lights[i].position.xyz - position;
 
            dist = length(light_direction);
            att = 1.0 / (lights[i].attenuation.x + lights[i].attenuation.y * dist + lights[i].attenuation.z * dist * dist);
        }
        
        float spot = 1;
        if(lights[i].position.w == 2){
            //spot
            vec3 from_light = position - lights[i].position.xyz;
            light_direction = normalize(lights[i].cone.xyz);
            cut_off = lights[i].cone.w;

            spot = dot(normalize(from_light), light_direction);
            if(spot < cut_off) continue;

            light_direction = -from_light;
        }


        float shadow = shadow_calc(shadow_pos, position, light_direction, normal);
        
        
        //diffuse
        float d = dot(normal, normalize(light_direction));
        //diffuse += d * lights[i].color.rgb * lights[i].color.a * att * spot * (1-shadow);
        diffuse = vec3(shadow);
    }

    //diffuse *= albedo;
    color = diffuse;

}

