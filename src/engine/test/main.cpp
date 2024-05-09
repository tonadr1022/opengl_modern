#include <engine/application/engine.h>
#include <engine/application/input.h>
#include <engine/timestep.h>
#include <spdlog/spdlog.h>

#include <tracy/Tracy.hpp>

#include "scene_main.h"

int main() {
  {
    ZoneScopedN("init");
    engine::Engine::Get().LoadScene(std::make_unique<SceneMain>());
  }
  engine::Engine::Get().Run();
  return 0;
}
