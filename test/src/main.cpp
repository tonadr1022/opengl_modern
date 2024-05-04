#include <engine/application/engine.h>
#include <engine/application/input.h>
#include <engine/timestep.h>

#include "scene_2.h"
#include "scene_main.h"

void LoadScenes(engine::Engine& engine) {
  auto scene = std::make_unique<SceneMain>();
  auto scene2 = std::make_unique<Scene2>();
  engine.AddScene(std::move(scene));
  engine.AddScene(std::move(scene2));
  engine.LoadScene("main");
}

int main() {
  LoadScenes(Engine::Get());
  Engine::Get().Run();
  return 0;
}
