#pragma once

#include <entt/core/hashed_string.hpp>
namespace engine {

struct MaterialData {
  entt::hashed_string shader_id;
  glm::vec3 diffuse;
  glm::vec3 roughness_metallic;
};

}  // namespace engine
