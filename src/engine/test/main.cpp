#include "engine/application/engine.h"
#include "scene_main.h"

int main() {
  {
    ZoneScopedN("init");
    engine::Engine::Get().LoadScene(std::make_unique<SceneMain>());
  }
  engine::Engine::Get().Run();
  return 0;
}
