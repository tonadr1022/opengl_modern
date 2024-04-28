#pragma once
#include <engine/application/engine.h>
#include <engine/application/input.h>
#include <engine/scene.h>
#include <engine/timestep.h>

class Scene2 : public Scene {
 public:
  Scene2() : Scene("test2") {}

  void Load() override { spdlog::info("loading scene 2."); };

  void OnUpdate(Timestep timestep) override {
    // std::cout << "scene2 update: " << timestep.delta_time << "\n";
  };

  void OnKeyEvent(const KeyEvent& e) override {
    bool pressed = e.action == InputAction::Press;
    if (pressed) {
      if (e.key == KeyCode::B) {
        Engine::Get().LoadScene("main");
      }
    }
  }
};
