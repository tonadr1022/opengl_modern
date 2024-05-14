#version 460 core
#extension GL_ARB_bindless_texture : enable

layout(location = 0) in VS_OUT {
    vec3 posWorldSpace;
    vec3 normal;
    vec2 texCoords;
    flat uint materialIndex;
} fs_in;

layout(std140, binding = 0) uniform Matrices
{
    mat4 vp_matrix;
    vec3 cam_pos;
};

uniform bool u_overrideMaterial;
uniform vec2 u_metallicRoughnessOverride;
uniform vec3 u_albedoOverride;

layout(location = 0) out vec4 o_color;

struct PointLight {
    vec3 position;
    vec3 color;
    float linear;
    float quadratic;
    // float radiusSquared;
};

layout(binding = 2, std430) readonly buffer PointLights {
    PointLight pointLights[];
};

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
    vec4 albedo;
    float roughness;
    float metallic;
    float ao;

    ///////////////////////// Assign material properties///////////////////////////////
    Material material = materials[fs_in.materialIndex];
    const bool hasAlbedo = (material.albedo_map_handle.x != 0 || material.albedo_map_handle.y != 0);
    const bool hasRoughness = (material.roughness_map_handle.x != 0 || material.roughness_map_handle.y != 0);
    const bool hasMetallic = (material.metalness_map_handle.x != 0 || material.metalness_map_handle.y != 0);
    const bool hasAO = (material.ao_map_handle.x != 0 || material.ao_map_handle.y != 0);
    const bool hasNormal = (material.normal_map_handle.x != 0 || material.normal_map_handle.y != 0);
    if (u_overrideMaterial) {
        albedo = vec4(u_albedoOverride, 1.0);
        roughness = u_metallicRoughnessOverride.x;
        metallic = u_metallicRoughnessOverride.y;
        ao = 1.0;
    } else {
        if (hasAlbedo) {
            albedo = texture(sampler2D(material.albedo_map_handle), fs_in.texCoords).rgba;
        } else {
            albedo = vec4(material.base_color, 1.0);
        }
        if (hasRoughness) {
            roughness = texture(sampler2D(material.roughness_map_handle), fs_in.texCoords).r;
        } else {
            roughness = material.metallic_roughness.x;
        }
        if (hasMetallic) {
            metallic = texture(sampler2D(material.metalness_map_handle), fs_in.texCoords).r;
        } else {
            metallic = material.metallic_roughness.y;
        }
        if (hasAO) {
            ao = texture(sampler2D(material.ao_map_handle), fs_in.texCoords).r;
        } else {
            ao = 1.0;
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////
    if (albedo.a < 0.1) {
        discard;
    }
    vec3 normal = normalize(fs_in.normal);
    vec3 toCam = normalize(cam_pos - fs_in.posWorldSpace);

    /////////////////////// Light calc ///////////////////////////////////
    vec3 light_out = vec3(0.0);
    for (int i = 0; i < pointLights.length(); i++) {
        vec3 toLight = normalize(pointLights[i].position - fs_in.posWorldSpace);
        vec3 halfVector = normalize(toCam + toLight);
        float distToLight = length(pointLights[i].position - fs_in.posWorldSpace);
        // inverse square law
        float attenuation = 1.0 / (distToLight);
        vec3 radiance = pointLights[i].color * attenuation;
    }

    o_color = albedo;
}
