#version 400

in vec2 uv;

uniform vec4 camera_position;

uniform sampler2D position_texture;
uniform sampler2D normal_texture;
uniform sampler2D color_texture;

#define MAX_LIGHTS 10
uniform int num_lights;
uniform struct Light{
    vec4 position;
    vec4 color;
    vec4 attenuation;
    vec4 cone;
} lights[MAX_LIGHTS];

out vec3 color;

void main(){


    vec3 position = vec3(texture(position_texture, uv));
    vec3 normal   = vec3(texture(normal_texture, uv));
    vec3 albedo  = vec3(texture(color_texture, uv));

    vec3 diffuse = vec3(0);

    vec3 view_direction = camera_position.xyz - position;

    float att = 1;

    float dist = -1;
    bool draw_white = false;
    for(int i = 0; i < num_lights; i++){
        vec3 light_direction;

        if(lights[i].position.w == 0){
            light_direction = -lights[i].position.xyz;
        }else if(lights[i].position.w == 1){
            light_direction = lights[i].position.xyz - position;
 
            dist = length(light_direction);
            att = 1.0 / (lights[i].attenuation.x + lights[i].attenuation.y * dist + lights[i].attenuation.z * dist * dist);
        }

        //diffuse
        float d = dot(normal, normalize(light_direction));
        diffuse += max(d, 0.0) * lights[i].color.rgb * lights[i].color.a * att;
        
        //specular


        if(dist != -1 && dist < 5){
            draw_white = true;
            continue;
        }

    }

    diffuse *= albedo;
    color = diffuse;

}
