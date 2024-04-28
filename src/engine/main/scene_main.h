#pragma once

#include <engine/application/engine.h>
#include <engine/ecs/component/transform.h>
#include <engine/scene.h>

#include "engine/timestep.h"

class SceneMain : public Scene {
 public:
  SceneMain();

  void OnUpdate(Timestep timestep) override;

  void OnKeyEvent(const KeyEvent& e) override;
};
