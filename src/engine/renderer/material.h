#pragma once

#include <entt/core/hashed_string.hpp>
#include <glm/vec3.hpp>

#include "engine/renderer/gl/texture_2d.h"

namespace engine {

namespace gfx {
class Texture2D;

struct BindlessMaterial {
  glm::vec3 base_color;
  uint64_t albedo_map_handle;
  uint64_t roughness_map_handle;
  uint64_t metalness_map_handle;
  uint64_t ao_map_handle;
  uint64_t normal_map_handle;
};

struct MaterialData {
  glm::vec3 base_color;
  gfx::Texture2D* albedo_texture{nullptr};
  gfx::Texture2D* roughness_texture{nullptr};
  gfx::Texture2D* metalness_texture{nullptr};
  gfx::Texture2D* ao_texture{nullptr};
  gfx::Texture2D* normal_texture{nullptr};
};

}  // namespace gfx

}  // namespace engine
