#pragma once
#include "engine/ecs/system/camera_system.h"
#include "engine/scene.h"

namespace engine {
struct Event;
}

class Scene2 : public engine::Scene {
 public:
  Scene2();
  void Init() override;
  void OnEvent(const engine::Event& e) override;
  void OnUpdate(Timestep timestep) override;

  engine::ecs::CameraSystem camera_system;

 private:
  entt::entity player_entity_{};
};
