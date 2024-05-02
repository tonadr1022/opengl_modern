#pragma once

namespace engine {

struct MaterialData {
  HashedString shader_id;
  glm::vec3 diffuse;
  glm::vec3 roughness_metallic;
};

}  // namespace engine
