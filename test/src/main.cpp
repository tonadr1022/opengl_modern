#include <engine/application/engine.h>
#include <engine/application/input.h>
#include <engine/timestep.h>

#include "scene_main.h"

int main() {
  Engine::Get().LoadScene(std::make_unique<SceneMain>());
  Engine::Get().Run();
  return 0;
}
