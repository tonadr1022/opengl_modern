#pragma once

#include <glm/fwd.hpp>
struct Event;
#include <glm/vec3.hpp>

namespace engine {

struct FPSCamera {
  glm::vec3 position{};
  glm::vec3 front{0.f, 0.f, -1.f};
  float yaw{-90.f}, pitch{0.f};
  float mouse_sensitivity{1};
  float fov{45};
  float near_plane{0.1f};
  float far_plane{1000.f};
  bool active{true};
  static constexpr const glm::vec3 UpVector{0.f, 1.f, 0.f};

  [[nodiscard]] glm::mat4 GetProjection(float aspect_ratio) const;
  [[nodiscard]] glm::mat4 GetView() const;
};

struct OrbitCamera {
  glm::vec3 position{};
  glm::vec3 front{};
};

enum class CameraMode { FPS, Orbit };

}  // namespace engine
