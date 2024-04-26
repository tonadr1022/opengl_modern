#version 410 core

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoords;

void main() {
    gl_Position = vec4(v_Position, 1.0);
}
