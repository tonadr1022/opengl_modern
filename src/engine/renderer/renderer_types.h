#pragma once

#include <glm/mat4x4.hpp>

namespace engine::gfx {

struct RenderViewInfo {
  glm::mat4 view_matrix;
  glm::mat4 projection_matrix;
};

}  // namespace engine::gfx
