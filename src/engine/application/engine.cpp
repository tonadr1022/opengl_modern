#include "engine.h"

#include "../scene.h"
#include "../timestep.h"
#include "../util/timer.h"
#include "engine/ecs/system/imgui_system.h"
#include "engine/ecs/system/window_system.h"
#include "input.h"

Engine* Engine::instance_ = nullptr;

Engine::Engine() {
  enabled_systems_.set();

  window_system_ = std::make_unique<WindowSystem>();
  imgui_system_ = std::make_unique<ImGuiSystem>();
  graphics_system_ = std::make_unique<GraphicsSystem>();

  window_system_->Init();
  window_system_->SetVsync(true);

  graphics_system_->Init();

  imgui_system_->Init(window_system_->GetContext());

  Input::init_glfw_input_callbacks(window_system_->GetContext());
}

void Engine::OnKeyEvent(KeyEvent& e) {
  // TODO(tony): imgui handle first
  EASSERT(active_scene_ != nullptr);
  active_scene_->OnKeyEvent(e);
}

Engine& Engine::Get() {
  if (instance_ == nullptr) {
    instance_ = new Engine;
  }
  EASSERT(instance_);
  return *instance_;
}

void Engine::Run() {
  EASSERT(active_scene_ != nullptr);
  running_ = true;

  Timestep timestep;
  util::Timer timer;
  timer.Start();
  double last_time = timer.GetElapsedSeconds();
  while (running_ && !window_system_->ShouldClose()) {
    Input::Update();

    auto current_time = timer.GetElapsedSeconds();
    timestep.dt_actual = current_time - last_time;
    last_time = current_time;

    imgui_system_->StartFrame();

    active_scene_->OnUpdate(timestep);
    graphics_system_->StartFrame();
    graphics_system_->EndFrame();

    imgui_system_->DebugMenu(timestep);
    imgui_system_->EndFrame();

    window_system_->SwapBuffers();
  }

  delete instance_;
}

void Engine::Shutdown() {
  imgui_system_->Shutdown();
  graphics_system_->Shutdown();
}

Engine::~Engine() = default;

void Engine::Stop() { running_ = false; }

void Engine::AddScene(std::unique_ptr<Scene> scene) {
  auto it = scenes_.find(scene->GetName());
  EASSERT_MSG(it == scenes_.end(), "Scene Added Already");
  scenes_.emplace(scene->GetName(), std::move(scene));
}

void Engine::LoadScene(const std::string& name) {
  auto it = scenes_.find(name);
  EASSERT_MSG(it != scenes_.end(), "Scene Not Found");
  active_scene_ = it->second.get();
  std::cout << "Loading scene: " << name << "\n";
}
