#include <engine/application/engine.h>
#include <engine/application/input.h>
#include <engine/timestep.h>

#include "engine/util/profiler.h"
#include "scene_main.h"

int main() {
  {
    PROFILE_SCOPE("Init");
    engine::Engine::Get().LoadScene(std::make_unique<SceneMain>());
  }
  {
    PROFILE_SCOPE("Run");
    engine::Engine::Get().Run();
  }
  return 0;
}
