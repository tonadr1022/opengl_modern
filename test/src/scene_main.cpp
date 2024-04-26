#include "scene_main.h"

#include <imgui.h>

#include "engine/application/key_codes.h"

SceneMain::SceneMain() : Scene("main") {
  auto tri = registry_.create();
  component::Transform t;
  t.SetTranslation({0.1, 0.1, 0});
  registry_.emplace<component::Transform>(tri, t);
}

void SceneMain::OnKeyEvent(const KeyEvent& e) {
  bool pressed = e.action == InputAction::Press;
  if (pressed) {
    if (e.key == KeyCode::Backspace && e.mods == KeyMod::Shift) {
      Engine::Get().Stop();
    }
    if (e.key == KeyCode::B) {
      Engine::Get().LoadScene("test2");
    }
  }
}

void SceneMain::OnUpdate(Timestep timestep) {}
