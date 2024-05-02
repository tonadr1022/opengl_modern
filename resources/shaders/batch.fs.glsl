#version 460 core

layout(location = 0) in VS_OUT {
    vec3 posWorldSpace;
    vec3 normal;
    vec2 texCoord;
} vs_in;

layout(location = 0) out vec4 o_color;

uniform vec3 diffuse;

void main() {
    o_color = vec4(1.0);
}
