#include "fps_camera.h"

#include <imgui.h>

#include "engine/application/event.h"
#include "engine/application/input.h"
#include "engine/timestep.h"

void FPSCamera::OnEvent(const Event& e) {
  switch (e.type) {
    case Event::Type::MouseScrolled:
      proj_mat_settings.fov += static_cast<float>(e.scroll.offset);
      proj_mat_settings.ClampFOV();
      break;
    case Event::Type::MouseMoved:
      OnMouseMoved(e.mouse_pos.x, e.mouse_pos.y);
      break;
    default:
      break;
  }
}

void FPSCamera::OnMouseMoved(float x_offset, float y_offset) {
  yaw += static_cast<float>(x_offset) * settings.mouse_sensitivity;
  pitch -= static_cast<float>(y_offset) *
           settings.mouse_sensitivity;  // need to flip yOffset (screen space)
  pitch = glm::clamp(pitch, -89.0f, 89.0f);
  glm::vec3 eulers;
  eulers.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
  eulers.y = glm::sin(glm::radians(pitch));
  eulers.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
  front = glm::normalize(eulers);
  right = glm::normalize(glm::cross(front, UpVector));
}

void FPSCamera::Update(Timestep timestep) {
  float offset = movement_speed * static_cast<float>(timestep.dt_actual);
  if (Input::IsKeyDown(KeyCode::W) || Input::IsKeyDown(KeyCode::O)) {
    position += front * offset;
  }
  if (Input::IsKeyDown(KeyCode::S) || Input::IsKeyDown(KeyCode::L)) {
    position -= front * offset;
  }
  if (Input::IsKeyDown(KeyCode::D) || Input::IsKeyDown(KeyCode::Semicolon)) {
    position += right * offset;
  }
  if (Input::IsKeyDown(KeyCode::A) || Input::IsKeyDown(KeyCode::K)) {
    position -= right * offset;
  }
  if (Input::IsKeyDown(KeyCode::U) || Input::IsKeyDown(KeyCode::R)) {
    position += UpVector * offset;
  }
  if (Input::IsKeyDown(KeyCode::J) || Input::IsKeyDown(KeyCode::F)) {
    position -= UpVector * offset;
  }
}

void FPSCamera::OnImGuiRender() {
  ImGui::Text("Position: %.1f, %.1f, %.1f", position.x, position.y, position.z);
  ImGui::Text("Front: %.1f, %.1f, %.1f", front.x, front.y, front.z);
  ImGui::SliderFloat("Movement Speed", &movement_speed, MinMoveSpeed, MaxMoveSpeed);
  settings.OnImGuiRender();
  proj_mat_settings.OnImGuiRender();
  // if (ImGui::Button("Reset Settings")) {
  //   m_movementSpeed = DefaultMovementSpeed;
  //   m_mouseSensitivity = DefaultMouseSensitivity;
  // }
}
