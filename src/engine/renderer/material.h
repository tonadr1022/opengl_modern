#pragma once

#include <entt/core/hashed_string.hpp>
#include <glm/vec3.hpp>

namespace engine {

struct Material {
  glm::vec3 base_color;
  uint32_t albedo_map;
};

struct BindlessMaterial {
  glm::vec3 base_color;
  uint64_t albedo_map_handle;
};

}  // namespace engine
