#pragma once

#include <cstdint>
#include <glm/vec3.hpp>

#include "engine/renderer/gl/texture_2d.h"

namespace engine {

namespace gfx {
class Texture2D;

// std430: https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)
struct alignas(16) BindlessMaterial {
  glm::vec3 albedo;
  float pad1;
  glm::vec2 metallic_roughness;
  glm::vec2 pad2;
  uint64_t albedo_map_handle{};
  uint64_t roughness_map_handle{};
  uint64_t metalness_map_handle{};
  uint64_t ao_map_handle{};
  uint64_t normal_map_handle{};
};

struct alignas(16) BindlessPhongMaterial {
  glm::vec3 albedo;
  float pad1;
  uint64_t albedo_map_handle{};
  uint64_t specular_map_handle{};
  uint64_t normal_map_handle{};
};

struct PhongMaterialData {
  glm::vec3 albedo{1, 0, 1};
  gfx::Texture2D* albedo_texture{nullptr};
  gfx::Texture2D* specular_texture{nullptr};
  gfx::Texture2D* normal_texture{nullptr};
};

struct MaterialData {
  glm::vec3 albedo{1, 1, 1};
  glm::vec2 metallic_roughness{0, 1};
  gfx::Texture2D* albedo_texture{nullptr};
  gfx::Texture2D* roughness_texture{nullptr};
  gfx::Texture2D* metalness_texture{nullptr};
  gfx::Texture2D* ao_texture{nullptr};
  gfx::Texture2D* normal_texture{nullptr};
};

}  // namespace gfx

}  // namespace engine
