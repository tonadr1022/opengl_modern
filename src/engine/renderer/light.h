#pragma once

#include <glm/vec3.hpp>

namespace engine {

struct DirectionalLight {
  glm::vec3 direction{0, 0, 0};

  // incredibly janky, but this makes using an ssbo possible. I couldn't figure out another way to
  // align properly
 private:
  float pad1_;

 public:
  glm::vec3 color{1, 1, 1};

 private:
  float pad2_;
};

struct PointLight {
  glm::vec3 position{0, 0, 0};
  // incredibly janky, but this makes using an ssbo possible. I couldn't figure out another way to
  // align properly
 private:
  float pad1_;

 public:
  glm::vec3 color{1, 1, 1};
  float intensity{10};
};

// static_assert(alignof(PointLight) == 16, "PointLight is not aligned to 16 bytes");
}  // namespace engine
