#pragma once
#include <engine/scene.h>

namespace engine {
struct Event;
}

class Scene2 : public engine::Scene {
 public:
  Scene2() = default;
  void OnEvent(const engine::Event& e) override;
};
