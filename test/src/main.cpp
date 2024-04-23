#include <GLFW/glfw3.h>
#include <engine/application.h>
#include <engine/input.h>
#include <engine/timestep.h>

#include <memory>

#include "engine/scene.h"

int count = 0;

void OnInit() {}

void OnUpdate(Timestep timestep) {
  count++;
  // std::cout << count << " " << std::fixed << std::setprecision(6) << timestep.delta_time << " "
  //           << timestep.elapsed_time << "\n";

  // if (count > 10) {
  //   Application::Quit();
  // }
}

void OnKeyEvent(int key, int action, int mods) {
  bool pressed = action == GLFW_PRESS;
  if (pressed) {
    if (key == GLFW_KEY_BACKSPACE && mods == GLFW_MOD_SHIFT) {
      Application::Quit();
    }
  }
}

void OnDraw() {}

int main() {
  Application::Init();
  Application::SetInitCallback(OnInit);
  Application::SetUpdateCallback(OnUpdate);
  Application::SetDrawCallback(OnDraw);
  Application::SetKeyEventCallback(OnKeyEvent);

  Application::AddScene(std::make_unique<Scene>("test"));
  Application::AddScene(std::make_unique<Scene>("test2"));
  Application::LoadScene("test2");

  Application::Run();

  Application::Shutdown();

  return 0;
}
