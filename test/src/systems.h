#pragma once

#include <glm/fwd.hpp>

struct Timestep;

namespace component {
struct FPSCamera;
}

namespace ecs {

namespace fps_cam_sys {
extern void OnUpdate(component::FPSCamera& camera, Timestep timestep);
extern void OnMouseMoved(component::FPSCamera& camera, glm::vec2 pos);
extern void OnScroll(component::FPSCamera& camera, float offset);
extern void OnImGui(component::FPSCamera& camera);
}  // namespace fps_cam_sys

};  // namespace ecs

// void FPSCamera::OnImGuiRender() {
//   ImGui::Text("Position: %.1f, %.1f, %.1f", position.x, position.y, position.z);
//   ImGui::Text("Front: %.1f, %.1f, %.1f", front.x, front.y, front.z);
//   ImGui::SliderFloat("Movement Speed", &movement_speed, MinMoveSpeed, MaxMoveSpeed);
//   settings.OnImGuiRender();
//   proj_mat_settings.OnImGuiRender();
//   // if (ImGui::Button("Reset Settings")) {
//   //   m_movementSpeed = DefaultMovementSpeed;
//   //   m_mouseSensitivity = DefaultMouseSensitivity;
//   // }
// }
