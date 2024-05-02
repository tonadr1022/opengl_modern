#version 460 core

layout(location = 0) in VS_OUT {
    vec3 posWorldSpace;
    vec3 normal;
    vec2 texCoord;
} vs_in;

layout(location = 0) out vec4 o_color;
struct Material {
    vec3 diffuse;
}
uniform Material material;

void main() {
    o_color = vec4(1.0);
}
