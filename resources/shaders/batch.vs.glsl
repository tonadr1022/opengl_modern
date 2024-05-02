#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 vp_matrix;

layout(location = 0) out VS_OUT {
    vec3 posWorldSpace;
    vec3 normal;
    vec2 texCoord;
} vs_out;

struct UniformData {
    mat4 model;
};

layout(std430, binding = 0) readonly buffer data {
    UniformData uniforms[];
};

void main(void) {
    mat4 model = uniforms[gl_BaseInstance + gl_InstanceID].model;
    vec4 posWorldSpace = vp_matrix * model * vec4(aPosition, 1.0);
    vs_out.posWorldSpace = vec3(posWorldSpace);
    vs_out.texCoord = aTexCoord;
    vs_out.normal = aNormal;
    gl_Position = posWorldSpace;
}
