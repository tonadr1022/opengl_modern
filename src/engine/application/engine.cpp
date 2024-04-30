#include "engine.h"

#include <imgui.h>

#include "../scene.h"
#include "../timestep.h"
#include "../util/timer.h"
#include "engine/application/event.h"
#include "engine/ecs/system/graphics_system.h"
#include "engine/ecs/system/imgui_system.h"
#include "engine/ecs/system/window_system.h"
#include "input.h"

// Engine* Engine::instance_ = nullptr;

Engine::Engine() {
  window_system_ = new WindowSystem;
  imgui_system_ = new ImGuiSystem;
  graphics_system_ = new GraphicsSystem;

  window_system_->Init(this);
  // TODO(tony): global variable system
  window_system_->SetVsync(true);

  graphics_system_->Init();

  imgui_system_->Init(window_system_->GetContext());

  Input::init_glfw_input_callbacks(window_system_->GetContext());
}

void Engine::OnEvent(const Event& e) {
  switch (e.type) {
    case Event::Type::KeyPressed:
      if (e.key.code == KeyCode::Q && e.key.system) {
        Stop();
        return;
      }
    default:
      break;
  }
  active_scene_->OnEvent(e);
}

// Engine& Engine::Get() {
//   if (instance_ == nullptr) {
//     instance_ = new Engine;
//   }
//   EASSERT(instance_);
//   return *instance_;
// }

void Engine::Run() {
  EASSERT(active_scene_ != nullptr);
  running_ = true;

  Timestep timestep;
  util::Timer timer;
  timer.Start();
  double last_time = timer.GetElapsedSeconds();
  // const double sim_time = 1.0 / 60.0;
  while (running_ && !window_system_->ShouldClose()) {
    Input::Update();

    imgui_system_->StartFrame();

    auto current_time = timer.GetElapsedSeconds();
    double delta_time = current_time - last_time;
    last_time = current_time;
    // for now, give scene simulated time. eventually, have two functions, update and fixed update,
    // or do all fixed update in physics system that doesn't exist.
    double frame_time = delta_time;
    // while (frame_time >= 0) {
    //   double dt = std::min(frame_time, sim_time);
    //   timestep.dt_actual = dt;
    //   frame_time -= dt;
    //   active_scene_->OnUpdate(timestep);
    // }
    active_scene_->OnUpdate(timestep);
    timestep.dt_actual = delta_time;

    graphics_system_->StartFrame();
    graphics_system_->DrawOpaque(*active_scene_);
    graphics_system_->EndFrame();

    ImGuiSystemPerFrame(timestep);
    imgui_system_->EndFrame();

    window_system_->SwapBuffers();
  }

  Shutdown();
}

void Engine::ImGuiSystemPerFrame(Timestep timestep) {
  ImGui::Begin("Settings");
  bool vsync = window_system_->GetVsync();
  if (ImGui::Checkbox("Vsync", &vsync)) {
    window_system_->SetVsync(vsync);
  }

  imgui_system_->FramerateSubMenu(timestep);

  ImGui::End();
}

void Engine::Shutdown() {
  imgui_system_->Shutdown();
  graphics_system_->Shutdown();
  window_system_->Shutdown();

  delete imgui_system_;
  delete graphics_system_;
  delete window_system_;
}

Engine::~Engine() = default;

void Engine::Stop() { running_ = false; }

void Engine::AddScene(std::unique_ptr<Scene> scene) {
  auto it = scenes_.find(scene->GetName());
  scene->engine_ = this;
  EASSERT_MSG(it == scenes_.end(), "Scene Added Already");
  scenes_.emplace(scene->GetName(), std::move(scene));
}

void Engine::LoadScene(const std::string& name) {
  auto it = scenes_.find(name);
  EASSERT_MSG(it != scenes_.end(), "Scene Not Found");
  std::cout << "Loading scene: " << name << "\n";
  if (active_scene_) active_scene_->Shutdown();
  active_scene_ = it->second.get();
  active_scene_->Load();
}
