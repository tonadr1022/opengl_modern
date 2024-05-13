#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

// std140 explicitly states the memory layout.
// https://registry.khronos.org/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
layout(std140, binding = 0) uniform Matrices
{
    mat4 vp_matrix;
};

layout(location = 0) out VS_OUT {
    vec3 posWorldSpace;
    vec3 normal;
    vec2 texCoord;
    flat uint materialIndex;
} vs_out;

struct UniformData {
    mat4 model;
    uint materialIndex;
};

layout(std430, binding = 0) readonly buffer data {
    UniformData uniforms[];
};

void main(void) {
    UniformData uniformData = uniforms[gl_DrawID];
    mat4 model = uniformData.model;
    vec4 posWorldSpace = vp_matrix * model * vec4(aPosition, 1.0);
    vs_out.posWorldSpace = vec3(posWorldSpace);
    vs_out.texCoord = aTexCoord;
    vs_out.normal = aNormal;
    vs_out.materialIndex = uniformData.materialIndex;
    gl_Position = posWorldSpace;
}
