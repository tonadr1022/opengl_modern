#version 460 core
#extension GL_ARB_bindless_texture : enable

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gRMA;

layout(location = 0) in VS_OUT {
    vec3 posWorldSpace;
    vec3 normal;
    vec2 texCoords;
    flat uint materialIndex;
} fs_in;

uniform bool u_overrideMaterial;
uniform vec2 u_metallicRoughnessOverride;
uniform vec3 u_albedoOverride;

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
    gNormal = fs_in.normal;
    if (!u_overrideMaterial) {
        const bool hasAlbedo = (material.albedo_map_handle.x != 0 || material.albedo_map_handle.y != 0);
        const bool hasRoughness = (material.roughness_map_handle.x != 0 || material.roughness_map_handle.y != 0);
        const bool hasMetallic = (material.metalness_map_handle.x != 0 || material.metalness_map_handle.y != 0);
        const bool hasAO = (material.ao_map_handle.x != 0 || material.ao_map_handle.y != 0);
        const bool hasNormal = (material.normal_map_handle.x != 0 || material.normal_map_handle.y != 0);
        if (hasAlbedo) {
            gAlbedo = texture(sampler2D(material.albedo_map_handle), fs_in.texCoords);
        } else {
            gAlbedo = vec4(material.base_color, 1.0);
        }
        if (hasRoughness) {
            gRMA.r = texture(sampler2D(material.roughness_map_handle), fs_in.texCoords).r;
        } else {
            gRMA.r = material.metallic_roughness.x;
        }
        if (hasMetallic) {
            gRMA.g = texture(sampler2D(material.metalness_map_handle), fs_in.texCoords).r;
        } else {
            gRMA.g = material.metallic_roughness.y;
        }
        if (hasNormal) {
            // // obtain normal from normal map in range [0,1]
            // gNormal = texture(sampler2D(material.normal_map_handle), vs_in.TexCoords).rgb;
            // // transform normal vector to range [-1,1]
            // gNormal = normalize(gNormal * 2.0 - 1.0);
            gNormal = normalize(fs_in.normal);
        } else {
            gNormal = normalize(fs_in.normal);
        }
        if (hasAO) {
            gRMA.b = texture(sampler2D(material.ao_map_handle), fs_in.texCoords).r;
        } else {
            gRMA.b = 1.0;
        }
    } else {
        gAlbedo = vec4(material.base_color, 1.0);
        gRMA.r = material.metallic_roughness.x;
        gRMA.g = material.metallic_roughness.y;
        gRMA.b = 1.0;
        gNormal = normalize(fs_in.normal);
    }
}
