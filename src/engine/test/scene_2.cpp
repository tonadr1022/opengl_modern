#include "scene_2.h"

#include "engine/application/event.h"
#include "scene_main.h"

void Scene2::OnEvent(const engine::Event& e) {
  switch (e.type) {
    case engine::EventType::KeyPressed:
      if (e.key.code == engine::KeyCode::B) {
        LoadScene(std::make_unique<SceneMain>());
      }
    default:
      break;
  }
}
