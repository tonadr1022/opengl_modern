#pragma once

#include <engine/application/engine.h>
#include <engine/ecs/component/transform.h>
#include <engine/scene.h>

#include "engine/timestep.h"

struct Player {};
class SceneMain : public Scene {
 public:
  SceneMain();
  void Load() override;
  void OnImGuiRender() override;
  void OnUpdate(Timestep timestep) override;
  void OnEvent(const Event& e) override;
  bool fps_focused_{true};
};
