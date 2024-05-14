#include "camera_system.h"

#include <imgui.h>

#include <entt/core/hashed_string.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#include "engine/application/engine.h"
#include "engine/application/input.h"
#include "engine/application/key_codes.h"
#include "engine/ecs/component/camera.h"
#include "engine/renderer/renderer_types.h"
#include "engine/window_manager.h"

namespace engine::ecs {

namespace {

void Scroll(FPSCamera &camera, float offset) {
  camera.fov_degrees += static_cast<float>(offset);
  camera.fov_degrees = std::clamp(camera.fov_degrees, 1.f, 200.f);
}

}  // namespace

CameraSystem::CameraSystem(entt::registry &registry, engine::gfx::RenderViewInfo &render_view_info)
    : ecs::ISystem(registry), render_view_info_(render_view_info) {}

void CameraSystem::OnUpdate(double timestep) {
  auto view = registry.view<FPSCamera>();
  if (camera_mode == engine::CameraMode::FPS) {
    view.each([this, timestep](FPSCamera &camera) {
      if (on_ && camera.active) {
        float movement_offset = fps_movement_speed_ * timestep;
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
      SetViewInfo(camera);
    });
  }
}

template <typename Camera>
void CameraSystem::SetViewInfo(Camera &camera) {
  render_view_info_.view_matrix = camera.GetView();
  auto dims = engine::WindowManager::Get().GetWindowDimensions();
  float aspect_ratio = dims.x / dims.y;
  render_view_info_.projection_matrix = camera.GetProjection(aspect_ratio);
  render_view_info_.cam_pos = camera.position;
}

void CameraSystem::InitDefaultCamera(entt::entity entity, const glm::vec3 &position,
                                     const glm::vec3 &front) {
  engine::FPSCamera camera;
  camera.position = position;
  camera.pitch = glm::asin(-front.y);
  camera.yaw = atan2(front.x, front.z);
  glm::vec3 front_actual;
  front_actual.x = glm::cos(glm::radians(camera.yaw)) * glm::cos(glm::radians(camera.pitch));
  front_actual.y = glm::sin(glm::radians(camera.pitch));
  front_actual.z = glm::sin(glm::radians(camera.yaw)) * glm::cos(glm::radians(camera.pitch));
  camera.front = glm::normalize(front_actual);

  auto &c = registry.emplace<engine::FPSCamera>(entity, camera);
  SetViewInfo(c);
}

void CameraSystem::OnImGui() {
  if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
    auto view = registry.view<FPSCamera>();
    view.each([this](FPSCamera &camera) {
      auto &position = camera.position;
      auto &front = camera.front;
      ImGui::Text("Yaw: %.1f, Pitch: %.1f", camera.yaw, camera.pitch);
      ImGui::Text("Position: %.1f, %.1f, %.1f", position.x, position.y, position.z);
      ImGui::Text("Front: %.2f, %.2f, %.2f", front.x, front.y, front.z);
      ImGui::SliderFloat("Movement Speed", &fps_movement_speed_, kMinMoveSpeed, kMaxMoveSpeed);
      ImGui::SliderFloat("FOV", &camera.fov_degrees, kMinFov, kMaxFov);
      ImGui::SliderFloat("Mouse Sensitivity", &camera.mouse_sensitivity, kMinMouseSensitivity,
                         kMaxMouseSensitivity);
    });
  }
}

}  // namespace engine::ecs
