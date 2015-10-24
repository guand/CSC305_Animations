#version 330 core
in vec3 vpoint;

const vec3 COLORS[3] = vec3[](
    vec3(0.0,0.0,0.0),
    vec3(0.0,0.0,0.0),
    vec3(0.0,0.0,0.0));
out vec3 fcolor;
uniform mat4 M;

mat4 S(float s){
    return mat4(mat3(s));
}

void main() {
#if 1
    gl_Position = S(0.25) * M * vec4(vpoint, 1.0);
#endif

    fcolor = COLORS[gl_VertexID];
}
