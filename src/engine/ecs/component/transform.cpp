#include "transform.h"

namespace engine::component {
glm::mat4 Transform::CalculateModel() {
  return glm::translate(glm::mat4(1), translation_) * glm::mat4_cast(rotation_) *
         glm::scale(glm::mat4(1), scale_);
}

void Transform::SetTranslation(const glm::vec3& vec) {
  translation_ = vec;
  dirty_ = true;
}

void Transform::SetRotation(const glm::mat4& mat) {
  rotation_ = glm::quat_cast(mat);
  dirty_ = true;
}

void Transform::SetRotation(const glm::quat& quat) {
  rotation_ = quat;
  dirty_ = true;
}

void Transform::SetScale(const glm::vec3& vec) {
  scale_ = vec;
  dirty_ = true;
}

}  // namespace engine::component
