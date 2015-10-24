#version 330 core
in vec3 vpoint;
in vec2 vtexcoord;
out vec2 uv;

uniform mat4 M;

mat4 S(float s){
    return mat4(mat3(s));
}

void main() {
    gl_Position = S(0.25) * M * vec4(vpoint, 1.0);
    uv = vtexcoord;
}
