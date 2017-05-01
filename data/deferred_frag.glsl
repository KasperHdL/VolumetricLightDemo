#version 400

in vec4 p;
in vec3 n;
in vec2 t;

uniform vec4 color;
uniform sampler2D tex;

layout(location = 0) out vec4 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 diffuse;

void main(){
    position = p;
    normal = normalize(n);
    diffuse = (color * texture(tex, t)).xyz;
}
