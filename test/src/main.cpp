#include <engine/application/engine.h>
#include <engine/application/input.h>
#include <engine/timestep.h>

#include "engine/ecs/entity/entity.h"
#include "engine/scene.h"

int count = 0;

void OnInit() {}

class Scene1 : public Scene {
 public:
  Scene1() : Scene("test1") {}
  void OnUpdate(Timestep timestep) override {
    std::cout << "scene1 update: " << timestep.delta_time << "\n";
  };

  void OnKeyEvent(const KeyEvent& e) {
    bool pressed = e.action == GLFW_PRESS;
    if (pressed) {
      if (e.key == GLFW_KEY_BACKSPACE && e.mods == GLFW_MOD_SHIFT) {
        Engine::Get().Stop();
      }
      if (e.key == GLFW_KEY_B) {
        Engine::Get().LoadScene("test2");
      }
    }
    std::cout << "key event\n";
  }
};

class Scene2 : public Scene {
 public:
  Scene2() : Scene("test2") {}
  void OnUpdate(Timestep timestep) override {
    std::cout << "scene2 update: " << timestep.delta_time << "\n";
  };
  void OnKeyEvent(const KeyEvent& e) {
    bool pressed = e.action == GLFW_PRESS;
    if (pressed) {
      if (e.key == GLFW_KEY_BACKSPACE && e.mods == GLFW_MOD_SHIFT) {
        Engine::Get().Stop();
      }
      if (e.key == GLFW_KEY_B) {
        Engine::Get().LoadScene("test1");
      }
    }
    std::cout << "key event\n";
  }
};

void OnKeyEvent(int key, int action, int mods) {}

void OnDraw() {}

void LoadScenes(Engine& engine) {
  auto scene = std::make_unique<Scene1>();
  auto scene2 = std::make_unique<Scene2>();
  engine.AddScene(std::move(scene));
  engine.AddScene(std::move(scene2));
  engine.LoadScene("test1");
}

int main() {
  auto* engine = new Engine;
  LoadScenes(*engine);
  engine->Run();
  delete engine;
  return 0;
}
