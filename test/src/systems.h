#pragma once

#include <entt/entity/fwd.hpp>
#include <glm/fwd.hpp>

namespace engine {
struct Timestep;
struct Event;
namespace component {
struct FPSCamera;
struct Transform;
}  // namespace component
}  // namespace engine

namespace ecs {

namespace fps_cam_sys {
extern void Run(entt::registry& registry, engine::Timestep timestep);
extern void FPSCamImGui(engine::component::FPSCamera& camera);
extern void OnEvent(entt::registry& registry, const engine::Event& e);
}  // namespace fps_cam_sys

namespace movement_system {}
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
