#version 400

uniform vec4 screen_size;

uniform sampler2DMS position_texture;
uniform sampler2DMS normal_texture;
uniform sampler2DMS color_texture;

uniform vec4 camera_position;

uniform vec4 light_position;
uniform vec4 light_color;
uniform vec4 light_attenuation;
uniform vec4 light_cone;

uniform sampler2DMS shadow_map;
uniform int  light_shadow_index;
uniform mat4 light_shadow_vp;

uniform float time;

out vec3 color;

float rand(float n){return fract(sin(n) * 43758.5453123);}

/////////////////
//Shadow Calc
/////////////////

float calc_shadows(int index, vec3 position, vec3 light_dir, vec3 normal){
    vec4 frag_from_light = light_shadow_vp * vec4(position,1);

    vec3 coord = frag_from_light.xyz / frag_from_light.w;
    coord = coord * 0.5 + 0.5;
    if(coord.z > 1.0) return 1.0;
    

    float bias = max(0.025 * (1.0 - dot(normal, normalize(light_dir))), 0.005);

    float shadow = 0.0;

    vec2 texel_size = 1.0  / textureSize(shadow_map, 0);

    for(int x = -1; x <= 1; x++){
        for(int y = -1; y <= 1; y++){
            float adj_depth = texelFetch(shadow_map, coord.xy + vec2(x,y) * texel_size, gl_SampleID).r;
            shadow +=  coord.z > adj_depth + bias ? 1.0 : 0.0;

        }
    }
    shadow /= 9;

    return shadow;

}


float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float calc_shadows(int index, vec3 position){
    vec4 frag_from_light = light_shadow_vp * vec4(position,1);

    vec3 coord = frag_from_light.xyz / frag_from_light.w;
    coord = coord * 0.5 + 0.5;
    if(coord.z > 1.0) return 1.0;
    

    float adj_depth = texelFetch(shadow_map, coord.xy, gl_SampleID).r;
    float shadow =  coord.z > adj_depth ? 1.0 : 0.0;

    return shadow;

}


/////////////////
//Light Calc
/////////////////

vec4 light_function(vec3 position){
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
        int n = int(1.5f * l);
        float f = l / n;


        dir = dir / l;

        float p = 0;

        color.r = 0;


        vec3 from_light = position - light_position.xyz;

        float spot = pow(max(dot(normalize(from_light), normalize(light_cone.xyz)),0), light_cone.w);
        float dist = length(from_light);

        float att = 1.0 / (light_attenuation.x + light_attenuation.y * dist + light_attenuation.z * dist * dist);

        float shadow = 0;

        float i =  spot * att;




        while(p < l){
            vec3 pos = start + p * (dir * rand(p * time));

            //calculate light for pos
            from_light = pos - light_position.xyz;
            spot = pow(max(dot(normalize(from_light), normalize(light_cone.xyz)),0), light_cone.w);
            dist = length(from_light);
            att = 1.0 / (light_attenuation.x + light_attenuation.y * dist + light_attenuation.z * dist * dist);

            shadow = 0;

            if(light_shadow_index >= 0)
                shadow = calc_shadows(light_shadow_index, pos);

            //add light
            color.r += spot * att * (1-shadow);

            //increment
            p += f;
        } 

        //divide by num samples
        color.r /= n;




        from_light = position - light_position.xyz;

        light = vec4(-from_light.xyz, i);

    }else{

        vec3 dir = position - camera_position.xyz;


        light = vec4(0,0,0, 1);
    }

    return light;
}


/////////////////
//Main
/////////////////


void main(){
    vec2 uv = gl_FragCoord.xy / screen_size.xy;
    vec3 position = vec3(texelFetch(position_texture , uv, gl_SampleID));
    vec3 normal   = vec3(texelFetch(normal_texture   , uv, gl_SampleID));
    vec3 albedo   = vec3(texelFetch(color_texture    , uv, gl_SampleID));

    //Calculate Light Contribution
        //xyz = light_direction
        //w   = contribution
    vec4 light = light_function(position);

    //Calculate Shadow
    float shadow = 0.0;

    if(light_shadow_index >= 0)
        shadow = calc_shadows(light_shadow_index, position, light.xyz, normal);


    //Calculate Frag
    float d = max(dot(normal, normalize(light.xyz)), 0.0);
    vec3 diffuse = d * (light_color.rgb * light_color.a * light.w * (1 - shadow) + light_color.rgb * light_color.a * color.r);

    color = diffuse * albedo;

    //adjust for exposure
    float exposure = camera_position.w;
    const float gamma = 2.2;

    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-color * exposure);
    // Gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));

    color = mapped;
}
