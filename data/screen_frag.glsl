#version 400

in vec2 uv;

uniform vec4 camera_position;

uniform sampler2D position_texture;
uniform sampler2D local_position_texture;
uniform sampler2D normal_texture;
uniform sampler2D color_texture;

uniform mat4 inverse_mvp;

uniform sampler2D shadow_map;
uniform mat4 shadow_vp;

#define MAX_LIGHTS 10
uniform int num_lights;
uniform struct Light{
    vec4 position;
    vec4 color;
    vec4 attenuation;
    vec4 cone;
} lights[MAX_LIGHTS];

out vec3 color;

float shadow_calc(vec3 position, vec3 local_position, vec3 light_dir, vec3 normal){
    vec4 frag_from_light = shadow_vp * vec4(position,1);

    vec3 coord = frag_from_light.xyz / frag_from_light.w;
    coord = coord * 0.5 + 0.5;
    if(coord.z > 1.0) return 1.0;
    

    float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);

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
    vec3 local_position = vec3(texture(local_position_texture , uv));
    vec3 normal         = vec3(texture(normal_texture         , uv));
    vec3 albedo         = vec3(texture(color_texture          , uv));

    vec3 diffuse = vec3(0);

    vec3 view_direction = camera_position.xyz - position;

    float att = 1;

    float dist;
    vec3 light_direction;
    float contribution;
    for(int i = 0; i < 1; i++){

        if(lights[i].position.w == 0){
            //directional
            light_direction = -lights[i].position.xyz;
            att = 1;
            contribution = 1;
        }else if(lights[i].position.w == 1){
            //point
            light_direction = lights[i].position.xyz - position;
 
            dist = length(light_direction);

            //attenuation
            contribution = 1.0 / (lights[i].attenuation.x + lights[i].attenuation.y * dist + lights[i].attenuation.z * dist * dist);

        }
        
        if(lights[i].position.w == 2){
            //spot
            vec3 from_light = position - lights[i].position.xyz;
            light_direction = normalize(lights[i].cone.xyz);

            float spot = pow(max(dot(normalize(from_light), light_direction),0), lights[i].cone.w);
            dist = length(from_light);

            float att = 1.0 / (lights[i].attenuation.x + lights[i].attenuation.y * dist + lights[i].attenuation.z * dist * dist);

            contribution = spot * att;
            light_direction = -from_light;
        }


        float shadow = shadow_calc(position, local_position, light_direction, normal);
        
        
        //diffuse
        float d = max(dot(normal, normalize(light_direction)), 0.0);
        diffuse += d * lights[i].color.rgb * lights[i].color.a * contribution * (1 - shadow);
    }

    diffuse *= albedo;
    color = diffuse;

}

