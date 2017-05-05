#version 400

uniform vec4 color;

out vec3 frag;

void main(){
    frag = color.rgb;
}
