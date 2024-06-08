#version 460 core

layout(location = 0) in vec3 aPosition;

uniform mat4 vp_matrix;

out vec3 WorldPos;

struct UniformData {
    mat4 model;
    mat4 normalMatrix;
    uint materialIndex;
};

layout(std430, binding = 0) readonly buffer data {
    UniformData uniforms[];
};

void main() {
    vec4 worldPos = uniforms[gl_DrawID].model * vec4(aPosition, 1.0);
    gl_Position = vp_matrix * worldPos;
    WorldPos = vec3(worldPos);
}
