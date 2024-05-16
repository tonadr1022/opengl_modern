#version 460 core

out vec4 o_color;

layout(location = 0) in vec2 TexCoords;

uniform sampler2D depthMap;

// uniform float near_plane;
// uniform float far_plane;
uniform bool perspective;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return 0;
    // TODO: finish
}

void main() {
    float depthVal = texture(depthMap, TexCoords).r;
    if (perspective) {
        o_color = vec4(vec3(LinearizeDepth(depthVal)), 1.0);
    } else {
        o_color = vec4(vec3(depthVal), 1.0);
    }
}
