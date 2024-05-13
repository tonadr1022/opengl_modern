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
  camera.fov += static_cast<float>(offset);
  camera.fov = std::clamp(camera.fov, 1.f, 200.f);
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
}

void CameraSystem::InitDefaultCamera(entt::entity entity) {
  engine::FPSCamera fps_cam;
  fps_cam.position = {0, 0, 1};
  auto &camera = registry.emplace<engine::FPSCamera>(entity, fps_cam);
  SetViewInfo(camera);
}

void CameraSystem::OnImGui() {
  auto view = registry.view<FPSCamera>();
  view.each([this](FPSCamera &camera) {
    auto &position = camera.position;
    auto &front = camera.front;
    ImGui::Text("Yaw: %.1f, Pitch: %.1f", camera.yaw, camera.pitch);
    ImGui::Text("Position: %.1f, %.1f, %.1f", position.x, position.y, position.z);
    ImGui::Text("Front: %.2f, %.2f, %.2f", front.x, front.y, front.z);
    ImGui::SliderFloat("Movement Speed", &fps_movement_speed_, MinMoveSpeed, MaxMoveSpeed);
    float fov_rad = glm::radians(camera.fov);
    if (ImGui::SliderAngle("FOV", &fov_rad, MinFov, MaxFov)) {
      camera.fov = glm::degrees(fov_rad);
    }
    ImGui::SliderFloat("Mouse Sensitivity", &camera.mouse_sensitivity, MinMouseSensitivity,
                       MaxMouseSensitivity);
  });
}

}  // namespace engine::ecs
