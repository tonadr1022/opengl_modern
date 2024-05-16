#version 460 core

layout(location = 0) in vec3 aPosition;

struct UniformData {
    mat4 model;
    mat4 normalMatrix;
    uint materialIndex;
};

layout(std430, binding = 0) readonly buffer data {
    UniformData uniforms[];
};

uniform mat4 u_lightSpaceMatrix;

void main() {
    // strictly to write to the depth buffer from perspective of directional light
    gl_Position = u_lightSpaceMatrix * uniforms[gl_DrawID].model * vec4(aPosition, 1.0);
}
