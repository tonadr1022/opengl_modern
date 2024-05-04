#include "systems.h"

#include <engine/application/engine.h>
#include <engine/application/input.h>
#include <engine/application/key_codes.h>
#include <engine/ecs/component/camera.h>
#include <engine/ecs/system/window_system.h>
#include <engine/pch.h>
#include <engine/timestep.h>
#include <imgui.h>

#include <entt/entity/registry.hpp>

#include "components.h"
#include "engine/application/event.h"

using namespace engine;
using namespace engine::component;

namespace ecs {

namespace fps_cam_sys {

void OnUpdate(FPSCamera &camera, engine::Timestep timestep) {
  float movement_offset = camera.movement_speed * static_cast<float>(timestep.dt_actual);
  glm::vec3 movement(0.f);
  if (Input::IsKeyDown(KeyCode::W) || Input::IsKeyDown(KeyCode::I)) {
    movement += camera.front;
  }
  if (Input::IsKeyDown(KeyCode::S) || Input::IsKeyDown(KeyCode::K)) {
    movement -= camera.front;
  }
  if (Input::IsKeyDown(KeyCode::D) || Input::IsKeyDown(KeyCode::L)) {
    movement += glm::normalize(glm::cross(camera.front, FPSCamera::UpVector));
  }
  if (Input::IsKeyDown(KeyCode::A) || Input::IsKeyDown(KeyCode::J)) {
    movement -= glm::normalize(glm::cross(camera.front, FPSCamera::UpVector));
  }
  if (Input::IsKeyDown(KeyCode::Y) || Input::IsKeyDown(KeyCode::R)) {
    movement += FPSCamera::UpVector;
  }
  if (Input::IsKeyDown(KeyCode::H) || Input::IsKeyDown(KeyCode::F)) {
    movement -= FPSCamera::UpVector;
  }
  if (glm::length(movement) > 0) {
    movement = glm::normalize(movement) * movement_offset;
    camera.position += movement;
  }

  auto cursor_offset = Input::GetCursorOffset();
  float mouse_offset = camera.mouse_sensitivity * 0.1;
  camera.yaw += cursor_offset.x * mouse_offset;
  camera.pitch = glm::clamp(camera.pitch - cursor_offset.y * mouse_offset, -89.0f, 89.0f);
  glm::vec3 front;
  front.x = glm::cos(glm::radians(camera.yaw)) * glm::cos(glm::radians(camera.pitch));
  front.y = glm::sin(glm::radians(camera.pitch));
  front.z = glm::sin(glm::radians(camera.yaw)) * glm::cos(glm::radians(camera.pitch));
  camera.front = glm::normalize(front);
}

void OnScroll(FPSCamera &camera, float offset) {
  camera.fov += static_cast<float>(offset);
  camera.fov = glm::clamp(camera.fov, 1.f, 200.f);
}

void OnImGui(FPSCamera &camera) {
  auto &position = camera.position;
  auto &front = camera.front;
  ImGui::Text("Yaw: %.1f, Pitch: %.1f", camera.yaw, camera.pitch);
  ImGui::Text("Position: %.1f, %.1f, %.1f", position.x, position.y, position.z);
  ImGui::Text("Front: %.2f, %.2f, %.2f", front.x, front.y, front.z);
  ImGui::SliderFloat("Movement Speed", &camera.movement_speed, FPSCamera::MinMoveSpeed,
                     FPSCamera::MaxMoveSpeed);
  float fov_rad = glm::radians(camera.fov);
  if (ImGui::SliderAngle("FOV", &fov_rad, FPSCamera::MinFov, FPSCamera::MaxFov)) {
    camera.fov = glm::degrees(fov_rad);
  }
  ImGui::SliderFloat("Mouse Sensitivity", &camera.mouse_sensitivity, FPSCamera::MinMouseSensitivity,
                     FPSCamera::MaxMouseSensitivity);
}

void OnEvent(entt::registry &registry, engine::Event &e) {
  auto player_entity = registry.view<Player>().front();
  auto &camera = registry.get<component::FPSCamera>(player_entity);
  auto &player = registry.get<Player>(player_entity);
  switch (e.type) {
    case EventType::KeyPressed:
      if (e.key.code == KeyCode::M) {
        player.fps_focused = !player.fps_focused;
        if (!player.fps_focused) engine::WindowSystem::Get().CenterCursor();
        WindowSystem::Get().SetCursorVisible(!player.fps_focused);
      } else if (e.key.code == KeyCode::B) {
        Engine::Get().LoadScene("test2");
      }
      break;
    case EventType::MouseScrolled:
      if (player.fps_focused) ecs::fps_cam_sys::OnScroll(camera, e.scroll.offset);
      break;
    default:
      break;
  }
}

}  // namespace fps_cam_sys
namespace move_system {}
}  // namespace ecs
