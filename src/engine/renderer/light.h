#pragma once

#include <glm/vec3.hpp>

namespace engine {

struct DirectionalLight {
  glm::vec3 direction;
  glm::vec3 color{1, 1, 1};
};

struct PointLight {
  glm::vec3 position;
  glm::vec3 color{1, 1, 1};
  float linear;
  float quadratic;
};

}  // namespace engine
