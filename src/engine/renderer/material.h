#pragma once

#include <cstdint>
#include <glm/vec3.hpp>

#include "engine/renderer/gl/texture_2d.h"

namespace engine {

namespace gfx {
class Texture2D;

// std430: https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)
struct alignas(16) BindlessMaterial {
  uint64_t albedo_map_handle{};
  uint64_t roughness_map_handle{};
  uint64_t metalness_map_handle{};
  uint64_t ao_map_handle{};
  uint64_t normal_map_handle{};
  // glm::vec3 albedo;
  // float pad1;
  // float roughness;
  // float metallic;
  // glm::vec2 pad2;
};

struct MaterialData {
  glm::vec3 albedo{1, 1, 1};
  float roughness{1};
  float metallic{0};
  gfx::Texture2D* albedo_texture{nullptr};
  gfx::Texture2D* roughness_texture{nullptr};
  gfx::Texture2D* metalness_texture{nullptr};
  gfx::Texture2D* ao_texture{nullptr};
  gfx::Texture2D* normal_texture{nullptr};
};

}  // namespace gfx

}  // namespace engine
