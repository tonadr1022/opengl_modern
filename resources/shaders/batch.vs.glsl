#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

// std140 explicitly states the memory layout.
// https://registry.khronos.org/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
layout(std140, binding = 0) uniform Matrices
{
    mat4 vp_matrix;
    vec3 cam_pos;
};

layout(location = 0) out VS_OUT {
    vec3 posWorldSpace;
    vec3 normal;
    vec2 texCoords;
    flat uint materialIndex;
} vs_out;

struct UniformData {
    mat4 model;
    mat4 normalMatrix;
    uint materialIndex;
};

struct Material {
    vec3 base_color;
    float pad1;
    float metallic;
    float roughness;
    float pad2;
    float pad3;
    uvec2 albedo_map_handle;
    uvec2 roughness_map_handle;
    uvec2 metalness_map_handle;
    uvec2 ao_map_handle;
    uvec2 normal_map_handle;
};

layout(std430, binding = 1) readonly buffer Materials {
    Material materials[];
};

layout(std430, binding = 0) readonly buffer data {
    UniformData uniforms[];
};

void main(void) {
    UniformData uniformData = uniforms[gl_DrawID];
    vs_out.posWorldSpace = vec3(uniformData.model * vec4(aPosition, 1.0));
    vs_out.texCoords = aTexCoords;
    vs_out.normal = mat3(uniformData.normalMatrix) * aNormal;
    vs_out.materialIndex = uniformData.materialIndex;
    Material material = materials[uniformData.materialIndex];
    const bool hasNormalMap = (material.normal_map_handle.x != 0 || material.normal_map_handle.y != 0);
    if (hasNormalMap) {} else {}

    gl_Position = vp_matrix * vec4(vs_out.posWorldSpace, 1.0);
}
