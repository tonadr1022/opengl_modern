#pragma once

#include "engine/camera/camera.h"
#include "engine/pch.h"

struct Timestep;
struct Event;

struct FPSCamera {
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 right;
  float yaw, pitch;

  glm::mat4 CalculateProjectionMatrix();
  glm::mat4 CalculateViewMatrix();
  glm::mat4 CalculateVPMatrix();

  void OnEvent(const Event& e);
  void Update(Timestep timestep);
  void OnImGuiRender();

  ProjectionMatrixSettings proj_mat_settings;
  CameraSettings settings;
  float movement_speed;
  static constexpr const float DefaultMovementSpeed = 5.0f;
  static constexpr const float MinMoveSpeed = 0.1f;
  static constexpr const float MaxMoveSpeed = 200.0f;
  static constexpr const glm::vec3 UpVector{0.f, 1.f, 0.f};

 private:
  void OnMouseMoved(float x_offset, float y_offset);
};
