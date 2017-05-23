#version 400

uniform vec4 screen_size;

uniform sampler2D position_texture;
uniform sampler2D normal_texture;
uniform sampler2D color_texture;
uniform sampler2D specular_texture;

uniform vec4 camera_position;

uniform vec4 light_position;
uniform vec4 light_color;
uniform vec4 light_attenuation;
uniform vec4 light_cone;

uniform sampler2D shadow_map;
uniform int  light_shadow_index;
uniform mat4 light_shadow_vp;

uniform float time;

uniform vec4 ray_att;
uniform float color_rand;

out vec3 result;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

/////////////////
//Shadow Calc
/////////////////

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



float calc_shadows(int index, vec3 position){
    vec4 frag_from_light = light_shadow_vp * vec4(position,1);

    vec3 coord = frag_from_light.xyz / frag_from_light.w;
    coord = coord * 0.5 + 0.5;
    if(coord.z > 1.0) return 1.0;
    
    float bias = 0.0005;

    float adj_depth = texture(shadow_map, coord.xy).r;
    float shadow =  coord.z > adj_depth + bias? 1.0 : 0.0;

    return shadow;

}


/////////////////
//Light Calc
/////////////////

vec4 light_function(vec3 position, vec3 normal, vec2 uv){
    vec4 light = vec4(1,1,1,0);

    if(light_position.w == 0){
        //directional
        light = vec4(-light_position.xyz, 1);

    }else if(light_position.w == 1){
        //point

        vec3 light_direction = light_position.xyz - position;

        float dist = length(light_direction);

        //attenuation
        float contribution = 1.0 / (light_attenuation.x + light_attenuation.y * dist + light_attenuation.z * dist * dist);

        light = vec4(light_direction.xyz, contribution);

    }else if(light_position.w == 2){
        //spot

        vec3 start = camera_position.xyz;
        vec3 dir = position - start;



        float l = length(dir);
        int n = 0; 


        dir = dir / l;

        float p = 0;

        result.r = 0;


        vec3 from_light = position - light_position.xyz;

        float spot = pow(max(dot(normalize(from_light), normalize(light_cone.xyz)),0), light_cone.w);
        float dist = length(from_light);

        float att = 1.0 / (light_attenuation.x + light_attenuation.y * dist + light_attenuation.z * dist * dist);

        float shadow = 0;

        float i =  spot * att;

        float f = 0;
        while(p < l && n < 1000){

            
            vec3 pos = start + (p + (rand(uv * p * time) - 0.5f)* f * ray_att.w) * dir;

            //calculate light for pos
            from_light = pos - light_position.xyz;
            dist = length(from_light);

            spot = pow(max(dot(normalize(from_light), normalize(light_cone.xyz)),0), light_cone.w);
            att = 1.0 / (light_attenuation.x + light_attenuation.y * dist + light_attenuation.z * dist * dist);

            shadow = 0;

            if(light_shadow_index >= 0)
                shadow = calc_shadows(light_shadow_index, pos);

            //add light
            float c = spot * att * (1-shadow);
            result.r += c;

            //increment
            f = ray_att.x + ray_att.y * p + ray_att.z * p * p;
            p += f;
            n++;
        } 

        //divide by num samples
        result.r /= n;




        from_light = position - light_position.xyz;

        light = vec4(-from_light.xyz, i);

    }

    return light;
}


/////////////////
//Main
/////////////////


void main(){
    vec2 uv       = gl_FragCoord.xy / screen_size.xy;
    vec3 position = vec3(texture(position_texture , uv));
    vec3 normal   = vec3(texture(normal_texture   , uv));
    vec3 color    = vec3(texture(color_texture    , uv));
    float specular = texture(specular_texture    , uv).r;

    //dithering
    float a = color_rand;
    color += vec3((rand(uv * time * 1)-.5f)*a);

    //Calculate Light Contribution
        //xyz = light_direction
        //w   = contribution
    vec4 light = light_function(position, normal, uv);

    //Calculate Shadow
    float shadow = 0.0;

    if(light_shadow_index >= 0)
        shadow = calc_shadows_smooth(light_shadow_index, position);


    //Calculate Frag
    float d = max(dot(normal, normalize(light.xyz)), 0.0);
    vec3 diffuse = d * (light_color.rgb * light_color.a * light.w * (1 - shadow)) + (light_color.rgb * light_color.a * result.r);
        //result.r is the volumetric light

    

    vec3 spec = vec3(0);
    if(specular > 0){
        vec3 halfway_dir = normalize(light.xyz + normalize(camera_position.xyz - position));
        spec = pow(max(dot(normal, halfway_dir), 0.0), specular) * light_color.rgb * light_color.a * light.w;
    }


    result = diffuse * color + spec * color;


}
