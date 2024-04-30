#pragma once

#include <engine/application/engine.h>
#include <engine/ecs/component/transform.h>
#include <engine/scene.h>

#include "engine/timestep.h"

class SceneMain : public Scene {
 public:
  SceneMain();

  void Load() override;

  void OnUpdate(Timestep timestep) override;
  void OnEvent(Event& e) override;
};
