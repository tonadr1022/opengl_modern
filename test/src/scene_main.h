#pragma once

#include <engine/application/engine.h>
#include <engine/ecs/component/transform.h>
#include <engine/scene.h>

#include "engine/timestep.h"

using namespace engine;

class SceneMain : public Scene {
 public:
  SceneMain();
  void OnImGuiRender() override;
  void OnUpdate(Timestep timestep) override;
  void OnEvent(const Event& e) override;
};
