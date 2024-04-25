#include "engine.h"

#include "../scene.h"
#include "../timestep.h"
#include "../util/timer.h"
#include "input.h"

Engine* Engine::instance_ = nullptr;

Engine::Engine() {
  graphics_system_ = std::make_unique<GraphicsSystem>();
  graphics_system_->Init();
  Input::init_glfw_input_callbacks(graphics_system_->GetWindow().GetContext());
}

Engine& Engine::Get() {
  if (instance_ == nullptr) {
    instance_ = new Engine;
  }
  EASSERT(instance_);
  return *instance_;
}

void Engine::Run() {
  running_ = true;

  Timestep timestep;
  util::Timer timer;
  timer.Start();
  double last_time = timer.GetElapsedSeconds();
  while (running_) {
    Input::Update();

    auto current_time = timer.GetElapsedSeconds();
    timestep.delta_time = current_time - last_time;
    last_time = current_time;
    timestep.elapsed_time = current_time;

    active_scene_->OnUpdate(timestep);

    graphics_system_->StartFrame();
    graphics_system_->EndFrame();
  }
}

Engine::~Engine() { graphics_system_->Shutdown(); }

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
}
