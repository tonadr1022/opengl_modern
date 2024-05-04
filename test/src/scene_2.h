#pragma once
#include <engine/scene.h>

namespace engine {
struct Event;
}

class Scene2 : public engine::Scene {
 public:
  Scene2() { spdlog::info("loading scene 2."); }

  void OnEvent(const engine::Event& e) override;
};
