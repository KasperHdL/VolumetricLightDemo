#version 400

uniform sampler2D position_texture;
uniform sampler2D normal_texture;
uniform sampler2D color_texture;

uniform vec4 camera_position;

uniform vec4 light_position;
uniform vec4 light_color;
uniform vec4 light_attenuation;
uniform vec4 light_cone;

uniform sampler2D shadow_map;
uniform int  light_shadow_index;
uniform mat4 light_shadow_vp;

out vec3 color;


/////////////////
//Light Calc
/////////////////

vec4 light_function(vec3 position){
    vec4 light = vec4(1,1,1,0);

    if(light_position.w == 0){
        light = vec4(-light_position.xyz, 1);

    }else if(light_position.w == 1){

        vec3 light_direction = light_position.xyz - position;

        float dist = length(light_direction);

        //attenuation
        float contribution = 1.0 / (light_attenuation.x + light_attenuation.y * dist + light_attenuation.z * dist * dist);

        light = vec4(light_direction.xyz, contribution);

    }else if(light_position.w == 2){

        vec3 from_light = position - light_position.xyz;

        float spot = pow(max(dot(normalize(from_light), normalize(light_cone.xyz)),0), light_cone.w);
        float dist = length(from_light);

        float att = 1.0 / (light_attenuation.x + light_attenuation.y * dist + light_attenuation.z * dist * dist);

        light = vec4(-from_light.xyz, spot * att);

    }else{
        light = vec4(1,1,1,1);
    }

    return light;
}


/////////////////
//Shadow Calc
/////////////////

float calc_shadows(int index, vec3 position, vec3 light_dir, vec3 normal){
    vec4 frag_from_light = light_shadow_vp * vec4(position,1);

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


/////////////////
//Main
/////////////////


void main(){
    vec3 position       = vec3(texture(position_texture , gl_FragCoord.xy));
    vec3 normal         = vec3(texture(normal_texture   , gl_FragCoord.xy));
    vec3 albedo         = vec3(texture(color_texture    , gl_FragCoord.xy));

    vec3 diffuse = vec3(0);

    vec3 view_direction = camera_position.xyz - position;

    //Calculate Light Contribution

    vec4 light = light_function(position);
        //xyz = light_direction
        //w   = contribution

    //Calculate Shadow
    float shadow = 0.0;

    if(light_shadow_index >= 0)
        shadow = calc_shadows(light_shadow_index, position, light.xyz, normal);


    //Calculate Frag

    float d = max(dot(normal, normalize(light.xyz)), 0.0);
    diffuse += d * light_color.rgb * light_color.a * light.w * (1 - shadow);

    color = diffuse * albedo;

    //@TEMP(KASPER)
    color = light_color.rgb;

}
