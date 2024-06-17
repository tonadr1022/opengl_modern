#pragma once

#include <glm/mat4x4.hpp>

namespace engine::gfx {

struct RenderViewInfo {
  glm::mat4 view_matrix;
  glm::mat4 projection_matrix;
  glm::vec3 cam_pos;
  bool dir_light_shadows_on{true};
  bool point_light_shadows_on{false};
};

}  // namespace engine::gfx
