#version 400

in vec2 uv;

uniform sampler2D rendered_texture;
uniform float time;


out vec3 color;

void main(){
    color = texture( rendered_texture, uv + 0.005*vec2( sin(time+1024.0*uv.x),cos(time+768.0*uv.y)) ).xyz;

}
