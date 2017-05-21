#version 400
layout(location = 0)in vec3 position;
layout(location = 1)in vec3 normal;
layout(location = 2)in vec4 uv;
layout(location = 3)in vec4 color;

out vec4 p;
out vec3 n;
out vec2 t;
out vec4 c;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void) {
    p = model * vec4(position, 1); 
    n = transpose(inverse(mat3(model))) * normal;

    t = uv.xy;
    c = color;

    gl_Position = projection * view * p;
}

