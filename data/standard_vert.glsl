#version 400
layout(location = 0)in vec3 position;
layout(location = 1)in vec3 normal;
layout(location = 2)in vec4 uv;
layout(location = 3)in vec4 color;

out vec4 p;
out vec3 n;
out vec2 t;
out vec4 e;
out vec4 c;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;

void main(void) {
    p = view * model * vec4(position, 1);
    n = mat3(view) * normal_matrix * normal;

    t = uv.xy;
    c = color;

    gl_Position = projection * p;
}

