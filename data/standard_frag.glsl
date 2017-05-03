#version 400
in vec4 p;
in vec3 n;
in vec2 t;
in vec4 e;
in vec4 c;

uniform vec4 ambientLight;
uniform vec4 color;
uniform sampler2D tex;

uniform vec4 lightPosType[4];
uniform vec4 lightColorRange[4];
uniform float specularity;

layout(location = 0)out vec3 fragColor;

void main(void)
{
    vec3 c = (n + vec3(1)) / 2;
    fragColor = n;
}
