#pragma once
#include <glm/vec3.hpp>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_coords;
};

using Index = uint32_t;
