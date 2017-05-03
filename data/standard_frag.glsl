#version 400
in vec4 p;
in vec3 n;
in vec2 t;
in vec4 e;
in vec4 c;

layout(location = 0)out vec3 fragColor;

void main(void)
{
    vec3 c = vec3(t,1);
    fragColor = c;
}
