#pragma once

#include <engine/application/engine.h>
#include <engine/ecs/component/transform.h>
#include <engine/scene.h>

#include "engine/timestep.h"
#include "systems.h"

class SceneMain : public engine::Scene {
 public:
  void Init() override;
  void OnImGuiRender() override;
  void OnEvent(const engine::Event& e) override;
  void OnUpdate(engine::Timestep timestep) override;

 private:
  ecs::CameraSystem camera_system_;
};
