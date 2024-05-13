#pragma once

#include <entt/entity/fwd.hpp>

#include "engine/application/engine.h"
#include "engine/ecs/system/camera_system.h"
#include "engine/scene.h"

class SceneMain : public engine::Scene {
 public:
  SceneMain();
  void Init() override;
  void OnImGuiRender() override;
  void OnEvent(const engine::Event& e) override;
  void OnUpdate(Timestep timestep) override;

  engine::ecs::CameraSystem camera_system;

 private:
  entt::entity player_entity_{};
};
