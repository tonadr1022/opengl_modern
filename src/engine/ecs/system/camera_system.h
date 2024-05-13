#pragma once

#include <entt/entity/fwd.hpp>
#include <glm/fwd.hpp>

#include "engine/ecs/component/camera.h"
#include "engine/ecs/system/isystem.h"

namespace engine {

namespace gfx {
struct RenderViewInfo;
}
namespace ecs {

struct CameraSystem : public engine::ecs::ISystem {
  explicit CameraSystem(entt::registry& registry, engine::gfx::RenderViewInfo& render_view_info);
  void OnUpdate(double timestep);
  void OnImGui();
  // camera system should always be enabled since camera always needs to update, but movement may
  // not always be on
  bool on_{false};
  float fps_movement_speed_{kDefaultMovementSpeed};

  engine::CameraMode camera_mode{engine::CameraMode::FPS};

  static constexpr const float kDefaultFov = 45.0f;
  static constexpr const float kMaxFov = 120.0f;
  static constexpr const float kMinFov = 5.0f;
  static constexpr const float kDefaultMouseSensitivity = 0.5f;
  static constexpr const float kMinMouseSensitivity = 0.05f;
  static constexpr const float kMaxMouseSensitivity = 1.0f;
  static constexpr const float kMinMoveSpeed = 0.1f;
  static constexpr const float kMaxMoveSpeed = 200.0f;
  static constexpr const float kDefaultMovementSpeed = 15.0f;
  void InitDefaultCamera(entt::entity entity, const glm::vec3& position = {0, 1, 1},
                         const glm::vec3& front = {0, 0, -1});

 private:
  template <typename Camera>
  void SetViewInfo(Camera& camera);

  engine::gfx::RenderViewInfo& render_view_info_;
};

};  // namespace ecs

}  // namespace engine
