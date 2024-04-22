#include <engine/application.h>

#include <iomanip>
#include <iostream>

int count = 0;

void OnInit() { std::cout << "init\n"; }

void OnUpdate(Timestep timestep) {
  count++;
  std::cout << count << " " << std::fixed << std::setprecision(6) << timestep.delta_time << " "
            << timestep.elapsed_time << "\n";

  if (count > 10) {
    Application::Quit();
  }
}

void OnDraw() { std::cout << "drawing\n"; }

int main() {
  Application::SetInitCallback(OnInit);
  Application::SetUpdateCallback(OnUpdate);
  Application::SetDrawCallback(OnDraw);

  Application::Init();
  Application::Run();
  Application::Shutdown();

  return 0;
}
