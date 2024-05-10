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
  float movement_speed{DefaultMovementSpeed};

  engine::CameraMode camera_mode{engine::CameraMode::FPS};

  static constexpr const float DefaultFov = 45.0f;
  static constexpr const float MaxFov = 120.0f;
  static constexpr const float MinFov = 5.0f;
  static constexpr const float DefaultMouseSensitivity = 0.5f;
  static constexpr const float MinMouseSensitivity = 0.05f;
  static constexpr const float MaxMouseSensitivity = 1.0f;
  static constexpr const float MinMoveSpeed = 0.1f;
  static constexpr const float MaxMoveSpeed = 200.0f;
  static constexpr const float DefaultMovementSpeed = 5.0f;
  void InitDefaultCamera(entt::entity entity);

 private:
  template <typename Camera>
  void SetViewInfo(Camera& camera);

  engine::gfx::RenderViewInfo& render_view_info_;
};

};  // namespace ecs

}  // namespace engine
