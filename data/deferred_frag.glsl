#version 150

in vec4 p;
in vec3 n;
in vec2 t;

uniform vec4 color;
uniform sampler2D tex;

out vec4 fragment[3];

void main(){
    fragment[0].rgb = p.xyz;
    fragment[1].rgb = normalize(n);
    fragment[2] = color * texture(tex, t);
}
