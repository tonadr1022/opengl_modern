#include "camera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

glm::mat4 engine::FPSCamera::GetProjection(float aspect_ratio) const {
  return glm::perspective(fov, aspect_ratio, near_plane, far_plane);
}

glm::mat4 engine::FPSCamera::GetView() const {
  return glm::lookAt(position, position + front, UpVector);
}
