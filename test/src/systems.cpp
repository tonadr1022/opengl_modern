#include "systems.h"

#include <engine/application/input.h>
#include <engine/application/key_codes.h>
#include <engine/ecs/component/camera.h>
#include <engine/timestep.h>
#include <imgui.h>

#include <glm/ext/matrix_clip_space.hpp>

namespace ecs {

namespace fps_cam_sys {

using namespace component;

void OnUpdate(FPSCamera &camera, Timestep timestep) {
  float offset = camera.movement_speed * static_cast<float>(timestep.dt_actual);
  if (Input::IsKeyDown(KeyCode::W) || Input::IsKeyDown(KeyCode::O)) {
    camera.position += camera.front * offset;
  }
  if (Input::IsKeyDown(KeyCode::S) || Input::IsKeyDown(KeyCode::L)) {
    camera.position -= camera.front * offset;
  }
  if (Input::IsKeyDown(KeyCode::D) || Input::IsKeyDown(KeyCode::Semicolon)) {
    camera.position += glm::normalize(glm::cross(camera.front, FPSCamera::UpVector)) * offset;
  }
  if (Input::IsKeyDown(KeyCode::A) || Input::IsKeyDown(KeyCode::K)) {
    camera.position -= glm::normalize(glm::cross(camera.front, FPSCamera::UpVector)) * offset;
  }
  if (Input::IsKeyDown(KeyCode::U) || Input::IsKeyDown(KeyCode::R)) {
    camera.position += FPSCamera::UpVector * offset;
  }
  if (Input::IsKeyDown(KeyCode::J) || Input::IsKeyDown(KeyCode::F)) {
    camera.position -= FPSCamera::UpVector * offset;
  }
}

void OnScroll(FPSCamera &camera, float offset) {
  camera.fov += static_cast<float>(offset);
  camera.fov = glm::clamp(camera.fov, 1.f, 200.f);
}

glm::mat4 GetView(FPSCamera &camera) {
  return glm::lookAt(camera.position, camera.position + camera.front, {0., 1., 0.});
}

glm::mat4 GetProjection(FPSCamera &camera, float aspect_ratio) {
  return glm::perspective(glm::radians(camera.fov), aspect_ratio, camera.near_plane,
                          camera.far_plane);
}

void OnMouseMoved(FPSCamera &camera, glm::vec2 pos) {
  static glm::vec2 last = pos;
  glm::vec2 offset = pos - last;
  last = pos;

  camera.yaw += static_cast<float>(offset.x) * camera.mouse_sensitivity;
  camera.pitch -= static_cast<float>(offset.y) *
                  camera.mouse_sensitivity;  // need to flip yOffset (screen space)
  camera.pitch = glm::clamp(camera.pitch, -89.0f, 89.0f);
  glm::vec3 eulers;
  eulers.x = glm::cos(glm::radians(camera.yaw)) * glm::cos(glm::radians(camera.pitch));
  eulers.y = glm::sin(glm::radians(camera.pitch));
  eulers.z = glm::sin(glm::radians(camera.yaw)) * glm::cos(glm::radians(camera.pitch));
  camera.front = glm::normalize(eulers);
}

void OnImGui(FPSCamera &camera) {
  auto &position = camera.position;
  auto &front = camera.front;

  ImGui::Text("Position: %.1f, %.1f, %.1f", position.x, position.y, position.z);
  ImGui::Text("Front: %.1f, %.1f, %.1f", front.x, front.y, front.z);
  ImGui::SliderFloat("Movement Speed", &camera.movement_speed, FPSCamera::MinMoveSpeed,
                     FPSCamera::MaxMoveSpeed);
  float fov_rad = glm::radians(camera.fov);
  if (ImGui::SliderAngle("FOV", &fov_rad, FPSCamera::MinFov, FPSCamera::MaxFov)) {
    camera.proj_mat_settings.fov = glm::degrees(fov_rad);
  }
  ImGui::Text("Aspect Ratio: %.3f", camera.aspect_ratio);
  ImGui::SliderFloat("Mouse Sensitivity", &camera.mouse_sensitivity, FPSCamera::MinMouseSensitivity,
                     FPSCamera::MaxMouseSensitivity);
}

}  // namespace fps_cam_sys
}  // namespace ecs
