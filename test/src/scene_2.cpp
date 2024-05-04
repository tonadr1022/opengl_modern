#include "scene_2.h"

#include <engine/application/event.h>

#include "scene_main.h"

void Scene2::OnEvent(const Event& e) {
  switch (e.type) {
    case EventType::KeyPressed:
      if (e.key.code == KeyCode::B) {
        Engine::Get().LoadScene(std::make_unique<SceneMain>());
      }
    default:
      break;
  }
}
