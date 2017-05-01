#version 400

uniform sampler2D position_texture;
uniform sampler2D normal_texture;
uniform sampler2D diffuse_texture;

out vec4 fragment;

void main(){
    fragment = texture(diffuse_texture, gl_FragCoord.xy);
}
