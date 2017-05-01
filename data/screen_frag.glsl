#version 150

uniform sampler2D gTex;
uniform sampler2D nTex;
uniform sampler2D cTex;

out vec4 fragment;

void main(){
    fragment = texture(nTex, gl_FragCoord.xy);
}
