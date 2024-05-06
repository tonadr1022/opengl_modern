#include "systems.h"

#include <engine/application/engine.h>
#include <engine/application/input.h>
#include <engine/application/key_codes.h>
#include <engine/ecs/component/camera.h>
#include <engine/ecs/system/window_system.h>
#include <engine/pch.h>
#include <engine/timestep.h>
#include <imgui.h>

#include <entt/core/hashed_string.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

#include "components.h"
#include "engine/application/event.h"
#include "scene_2.h"

using namespace engine;
using namespace engine::component;

namespace ecs {

namespace {
void OnScroll(FPSCamera &camera, float offset) {
  camera.fov += static_cast<float>(offset);
  camera.fov = glm::clamp(camera.fov, 1.f, 200.f);
}

void UpdateMatrices(entt::registry &registry, FPSCamera &camera) {
  auto matrices_view = registry.view<entt::tag<entt::hashed_string{"view_info"}>>();
  auto &camera_matrices = registry.get<engine::gfx::RenderViewInfo>(matrices_view.front());

  camera_matrices.view_matrix =
      glm::lookAt(camera.position, camera.position + camera.front, {0., 1., 0.});
  auto *window_manager = registry.ctx().get<WindowSystem *>();
  auto dims = window_manager->GetWindowDimensions();
  float aspect_ratio = static_cast<float>(dims.x) / static_cast<float>(dims.y);
  camera_matrices.projection_matrix =
      glm::perspective(glm::radians(camera.fov), aspect_ratio, camera.near_plane, camera.far_plane);
}

}  // namespace

void CameraSystem::OnUpdate(entt::registry &registry, engine::Timestep timestep) {
  entt::entity player_entity = registry.view<Player>().front();
  auto &player = registry.get<Player>(player_entity);
  if (!player.fps_focused) return;
  auto &camera = registry.get<component::FPSCamera>(player_entity);

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

  UpdateMatrices(registry, camera);
}

void CameraSystem::OnImGui(entt::registry &registry) {
  auto player_entity = registry.view<Player>().front();
  auto &camera = registry.get<component::FPSCamera>(player_entity);
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

bool CameraSystem::OnEvent(entt::registry &registry, const engine::Event &e) {
  auto player_entity = registry.view<Player>().front();
  auto &camera = registry.get<component::FPSCamera>(player_entity);
  auto &player = registry.get<Player>(player_entity);
  switch (e.type) {
    case EventType::KeyPressed:
      if (e.key.code == KeyCode::M) {
        player.fps_focused = !player.fps_focused;
        auto *window_manager = registry.ctx().get<WindowSystem *>();
        if (!player.fps_focused) window_manager->CenterCursor();
        window_manager->SetCursorVisible(!player.fps_focused);
        return true;
      } else if (e.key.code == KeyCode::B) {
        Engine::Get().LoadScene(std::make_unique<Scene2>());
        return true;
      }
      return false;
    case EventType::MouseScrolled:
      if (player.fps_focused) OnScroll(camera, e.scroll.offset);
      return true;
    default:
      return false;
  }
}

}  // namespace ecs
