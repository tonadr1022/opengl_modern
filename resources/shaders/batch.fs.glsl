#version 460 core
#extension GL_ARB_bindless_texture : enable

layout(location = 0) in VS_OUT {
    mat3 TBN;
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

uniform bool u_normalMapOn;
uniform bool u_roughnessMapOn;
uniform bool u_metallicMapOn;
uniform bool u_overrideMaterial;
uniform vec2 u_metallicRoughnessOverride;
uniform vec3 u_albedoOverride;
uniform bool u_directionalOn;
uniform vec3 u_directionalDirection;
uniform vec3 u_directionalColor;

layout(location = 0) out vec4 o_color;

struct PointLight {
    vec3 position;
    float _pad1;
    vec3 color;
    float intensity;
};

layout(binding = 2, std430) readonly buffer PointLights {
    PointLight pointLights[];
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

// see comments below
vec3 FresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 normal, vec3 halfVector, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

const float PI = 3.14159265358979323846;

void main() {
    // o_color = vec4(1.0);
    // return;

    vec4 albedo;
    float roughness;
    float metallic;
    float ao;
    vec3 normal;

    ///////////////////////// Assign material properties ///////////////////////////////
    Material material = materials[fs_in.materialIndex];
    const bool hasAlbedo = (material.albedo_map_handle.x != 0 || material.albedo_map_handle.y != 0);
    const bool hasRoughness = (material.roughness_map_handle.x != 0 || material.roughness_map_handle.y != 0);
    const bool hasMetallic = (material.metalness_map_handle.x != 0 || material.metalness_map_handle.y != 0);
    const bool hasAO = (material.ao_map_handle.x != 0 || material.ao_map_handle.y != 0);
    const bool hasNormalMap = (material.normal_map_handle.x != 0 || material.normal_map_handle.y != 0);
    if (u_overrideMaterial) {
        albedo = vec4(u_albedoOverride, 1.0);
        metallic = u_metallicRoughnessOverride.x;
        roughness = u_metallicRoughnessOverride.y;
        normal = normalize(fs_in.normal);
        ao = 1.0;
    } else {
        if (hasAlbedo) {
            albedo = texture(sampler2D(material.albedo_map_handle), fs_in.texCoords).rgba;
        } else {
            albedo = vec4(material.base_color, 1.0);
            // o_color = albedo;
            // return;
        }
        if (u_roughnessMapOn && hasRoughness) {
            // TODO: make more robust, not all maps will combine roughness and metallic this way.
            roughness = texture(sampler2D(material.roughness_map_handle), fs_in.texCoords).g;
        } else {
            roughness = material.roughness;
            // o_color = vec4(roughness, roughness, roughness, 1.0);
            // return;
        }
        if (u_metallicMapOn && hasMetallic) {
            metallic = texture(sampler2D(material.roughness_map_handle), fs_in.texCoords).r;
            // o_color = vec4(metallic, metallic, metallic, 1.0);
            // return;
        } else {
            metallic = material.metallic;
            // o_color = vec4(metallic, metallic, metallic, 1.0);
            // return;
        }
        if (hasAO) {
            ao = texture(sampler2D(material.ao_map_handle), fs_in.texCoords).r;
        } else {
            ao = 1.0;
        }
        if (u_normalMapOn && hasNormalMap) {
            // sample normal texture
            normal = texture(sampler2D(material.normal_map_handle), fs_in.texCoords).rgb;
            // transform it to [-1,1] range
            normal = normal * 2.0 - 1.0;
            // apply tbn matrix to convert it from tangent space to world space
            normal = normalize(fs_in.TBN * normal);
        } else {
            // use vertex interpolated normal
            normal = normalize(fs_in.normal);
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////
    if (albedo.a < 0.1) {
        discard;
    }

    vec3 V = normalize(cam_pos - fs_in.posWorldSpace);

    // reflectance at normal incidence
    // 0.04 for dielectrics, higher for metals
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo.rgb, metallic);

    vec3 light_out = vec3(0.0);
    for (int i = 0; i < 4; i++) {
        // per light radiance
        vec3 L = normalize(pointLights[i].position.xyz - fs_in.posWorldSpace);
        vec3 H = normalize(V + L);
        float distToLight = length(pointLights[i].position.xyz - fs_in.posWorldSpace);
        // inverse square law, more physically correct than linear-quadratic
        // float attenuation = 1.0 / (distToLight);
        float attenuation = 1.0 / (distToLight * distToLight);
        vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;

        float NDF = DistributionGGX(normal, H, roughness);
        float G = GeometrySmith(normal, V, L, roughness);
        // F: Fresnel-Schlick
        // ratio betwen specular and diffuse reflection.
        // clamp cosTheta to at least 0.
        vec3 F = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

        // Cook-Torrance specular BRDF: DFG/(4*(wo*n)(wi*n))
        vec3 numerator = NDF * G * F;
        // add epsilon to avoid divide by zero.
        float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        // specular contribution is the same as the Fresnel value
        vec3 kS = F;
        // for energy conservation, diffuse and specular light mustn't exceed 1 unless emits light.
        // Thus, kD is the remains of kS.
        vec3 kD = vec3(1.0) - kS;
        // metallic surfaces don't refract light, thus don't have diffuse reflections, so nullify diffuse component
        // the more metallic the surface. Linear blend
        kD *= 1.0 - metallic;

        // scale the light by NdotL
        float NdotL = max(dot(normal, L), 0.0);

        // add to outgoing radiance
        light_out += (kD * albedo.rgb / PI + specular) * radiance * NdotL;
    }

    if (u_directionalOn) {
        // directional
        vec3 L = normalize(u_directionalDirection);
        vec3 H = normalize(V + L);
        vec3 radiance = u_directionalColor;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(normal, H, roughness);
        float G = GeometrySmith(normal, V, L, roughness);
        vec3 F = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;
        // scale light by NdotL
        float NdotL = max(dot(normal, L), 0.0);
        // add to outgoing radiance Lo
        light_out += (kD * albedo.rgb / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    // TODO: replace ambient with IBL
    vec3 color = light_out;
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0 / 2.2));

    o_color = vec4(color, 1.0);

    // // color = pow(color, vec3(1.0 / 2.2));
    // o_color = vec4(color, 1.0);
}

// F0: surface reflection at zero incidence - how much surface reflects if looking directly at the surface.
// varies per material. tinted on metals. Common practice is to use 0.04 for dielectrics
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// D in the Cook-Torrance BRDF: normal Distribution function.
// approximates the amount of the surface's microfacets that are aligned to the halfway vector, influenced by
// surface roughness
// This is the Trowbridge-Reitz GGX.
// when roughness is low, a highly concentrated number of microfacets are aligned to halfway vectors over a small radius
// causing a bright spot and darker over the other vectors. On a rough surface, microfacets are aligned in more
// random directions, so a much large number of halfway vectors are relatively aligned to the microfacets, but less concentrated,
// resulting in a more diffuse appearance
// roughness is used here because term specific modifications can be made.
float DistributionGGX(vec3 normal, vec3 halfVector, float roughness) {
    // Disney and Epic Games observed that squaring the roughness in the geometry and normal distribution function is the best
    // value for a.
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(normal, halfVector), 0.0);
    float NdotH2 = NdotH * NdotH;
    float numerator = a2;
    float denominator = (NdotH2 * (a2 - 1.0) + 1.0);
    denominator = PI * denominator * denominator;
    return numerator / denominator;
}

// approximates relative surface area where its micro surface details overshadow eachother, causing light rays to be
// excluded. 1.0 measures no shadowing, and 0.0 measures complete shadowing. Intuitively, there is more shadowing
// when cos(theta) is closer to 0 compared to looking at a surface straight on.
float GeometrySchlickGGX(float NdotV, float roughness) {
    // remap roughness for direct lighting. Why? Beyond me at this point
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    // rougher surfaces have a higher probability of overshadowing microfacets
    float numerator = NdotV;
    float denominator = NdotV * (1.0 - k) + k;
    return numerator / denominator;
}

// to approximate geometry, need to take view direction (geometry obstruction) and light direction (geometry shadowing)
// into account by multiplying the two calculations.
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    return ggx1 * ggx2;
}
