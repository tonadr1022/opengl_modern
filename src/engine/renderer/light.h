#pragma once

#include <glm/vec4.hpp>

namespace engine {

struct DirectionalLight {
  glm::vec4 direction;
  glm::vec4 color{1, 1, 1, 0};
};

struct PointLight {
  glm::vec4 position{};
  glm::vec4 color{1, 1, 1, 0};
};

}  // namespace engine
