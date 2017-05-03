#version 400

in vec2 uv;

uniform sampler2D rendered_texture;
uniform float time;


out vec3 color;

void main(){
    color = texture( rendered_texture, uv ).xyz;

}
