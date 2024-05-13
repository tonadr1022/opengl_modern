#version 460 core
#extension GL_ARB_bindless_texture : enable

layout(location = 0) in VS_OUT {
    vec3 posWorldSpace;
    vec3 normal;
    vec2 texCoord;
    flat uint materialIndex;
} fs_in;

uniform bool u_overrideMaterial;
uniform vec2 u_metallicRoughnessOverride;
uniform vec3 u_albedoOverride;

layout(location = 0) out vec4 o_color;

struct Material {
    vec3 base_color;
    float pad1;
    vec2 metallic_roughness;
    float pad2;
    uvec2 albedo_map_handle;
    uvec2 roughness_map_handle;
    uvec2 metalness_map_handle;
    uvec2 ao_map_handle;
    uvec2 normal_map_handle;
};

layout(binding = 1, std430) readonly buffer Materials {
    Material materials[];
};

void main() {
    Material material = materials[fs_in.materialIndex];
    const bool hasAlbedo = (material.albedo_map_handle.x != 0 || material.albedo_map_handle.y != 0);
    if (u_overrideMaterial) {
        o_color = vec4(u_albedoOverride, 1.0);
    } else if (hasAlbedo) {
        o_color = texture(sampler2D(material.albedo_map_handle), fs_in.texCoord).rgba;
    } else {
        o_color = vec4(material.base_color, 1.0);
    }
}
