#pragma once

#include <glm/mat4x4.hpp>

namespace engine::gfx {

struct RenderViewInfo {
  glm::mat4 view_matrix;
  glm::mat4 projection_matrix;
  glm::vec3 cam_pos;
  bool dir_light_on{true};
  bool shadows{true};
};

}  // namespace engine::gfx
