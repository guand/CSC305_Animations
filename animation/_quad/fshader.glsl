#version 330 core
out vec4 color;
in vec2 uv;
uniform sampler2D tex;

void main() {
    color = texture(tex,uv).rgba;
//    for (int i = 1; i < 5; ++i)
//    {
//        vec2 offset =
//    }
}


