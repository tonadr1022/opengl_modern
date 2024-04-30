#version 460 core

layout(location = 0) in VS_OUT {
    vec3 posWorldSpace;
    vec3 normal;
    vec2 texCoord;
} vs_in;

layout(location = 0) out vec4 o_color;

void main() {
    o_color = vec4(vs_in.normal, 1.0);
}
