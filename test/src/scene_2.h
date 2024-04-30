#pragma once
#include <engine/application/engine.h>
#include <engine/application/event.h>
#include <engine/application/input.h>
#include <engine/scene.h>
#include <engine/timestep.h>

class Scene2 : public Scene {
 public:
  Scene2() : Scene("test2") {}

  void Load() override { spdlog::info("loading scene 2."); };

  void OnUpdate(Timestep timestep) override{
      // std::cout << "scene2 update: " << timestep.delta_time << "\n";
  };

  void OnEvent(const Event& e) override {
    switch (e.type) {
      case Event::KeyPressed:
        if (e.key.code == KeyCode::B) {
          engine_->LoadScene("main");
        }
      default:
        break;
    }
  }
};
