#pragma once

#include <engine/application/engine.h>
#include <engine/ecs/component/transform.h>
#include <engine/scene.h>

#include "camera_system.h"
#include "engine/timestep.h"

class SceneMain : public Scene {
 public:
  SceneMain();

  void Load() override;

  void OnUpdate(Timestep timestep) override;
  void OnEvent(const Event& e) override;
  CameraSystem fps_camera_system;
};
