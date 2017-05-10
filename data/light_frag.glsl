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

//////////////////
//Subroutines
//////////////////

/////////////////
//Light Calc
subroutine vec4 light_calculation(vec3); 

subroutine (light_calculation) vec4 directional(vec3 position){
        return vec4(-light_position.xyz, 1);
}

subroutine (light_calculation) vec4 point(vec3 position){

        vec3 light_direction = light_position.xyz - position;

        float dist = length(light_direction);

        //attenuation
        float contribution = 1.0 / (light_attenuation.x + light_attenuation.y * dist + light_attenuation.z * dist * dist);

        return vec4(light_direction.xyz, contribution);
}


subroutine (light_calculation) vec4 spot(vec3 position){
        vec3 from_light = position - light_position.xyz;

        float spot = pow(max(dot(normalize(from_light), normalize(light_cone.xyz)),0), light_cone.w);
        float dist = length(from_light);

        float att = 1.0 / (light_attenuation.x + light_attenuation.y * dist + light_attenuation.z * dist * dist);

        return vec4(-from_light.xyz, spot * att);
}


/////////////////
//Shadow Calc
subroutine float shadow_calculation(int, vec3, vec3, vec3);

subroutine (shadow_calculation) float no_shadows(int index, vec3 position, vec3 light_dir, vec3 normal){
    return 0.0;
}

subroutine (shadow_calculation) float create_shadows(int index, vec3 position, vec3 light_dir, vec3 normal){
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


subroutine uniform light_calculation light_function;
subroutine uniform shadow_calculation shadow_function;


/////////////////
//Main
/////////////////


void main(){
/*
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
    float shadow = shadow_function(light_shadow_index, position, light.xyz, normal);


    //Calculate Frag

    float d = max(dot(normal, normalize(light.xyz)), 0.0);
    diffuse += d * light_color.rgb * light_color.a * light.w * (1 - shadow);

    color = diffuse * albedo;
    */
    color = light_color.rgb;

}
