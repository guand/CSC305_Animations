#version 330 core

in vec3 position;
uniform mat4 MVP;
in vec2 vtexcoord;
out vec2 uv;

void main() {
    gl_Position = MVP * vec4(position, 1.0);
    uv = vtexcoord;
}

