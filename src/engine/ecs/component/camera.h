#pragma once

struct Timestep;
struct Event;

#include "engine/pch.h"

namespace component {

struct FPSCamera {
  glm::vec3 position{};
  glm::vec3 front{0.f, 0.f, -1.f};
  float yaw{-90.f}, pitch{0.f};
  float mouse_sensitivity{DefaultMouseSensitivity};
  float movement_speed{DefaultMovementSpeed};
  float fov{DefaultFov};
  float near_plane{0.1f};
  float far_plane{1000.f};
  static constexpr const float DefaultFov = 45.0f;
  static constexpr const float MaxFov = 120.0f;
  static constexpr const float MinFov = 5.0f;
  static constexpr const float DefaultMouseSensitivity = 0.1f;
  static constexpr const float MinMouseSensitivity = 0.01f;
  static constexpr const float MaxMouseSensitivity = 1.0f;
  static constexpr const float DefaultMovementSpeed = 5.0f;
  static constexpr const float MinMoveSpeed = 0.1f;
  static constexpr const float MaxMoveSpeed = 200.0f;
  static constexpr const glm::vec3 UpVector{0.f, 1.f, 0.f};
};

}  // namespace component
