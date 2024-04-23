#include "engine.h"

#include <memory>

#include "e_assert.h"
#include "ecs/system/graphics_system.h"
#include "input.h"
#include "scene.h"
#include "timestep.h"
#include "util/timer.h"

Engine::Engine() {
  graphics_system_ = std::make_unique<GraphicsSystem>();

  graphics_system_->Init();
  Input::init_glfw_input_callbacks(graphics_system_->GetWindow().GetContext());

  running_ = true;
}

void Engine::Run() {
  EASSERT_MSG(update_callback_ != nullptr, "Update callback");
  EASSERT_MSG(draw_callback_ != nullptr, "Draw callback");
  EASSERT_MSG(init_callback_ != nullptr, "Init callback");
  EASSERT_MSG(key_event_callback_ != nullptr, "Key callback");

  Timestep timestep;
  Timer timer;
  timer.Start();
  double last_time = timer.GetElapsedSeconds();

  while (running_) {
    Input::Update();

    auto current_time = timer.GetElapsedSeconds();
    timestep.delta_time = current_time - last_time;
    last_time = current_time;
    timestep.elapsed_time = current_time;

    graphics_system_->StartFrame();

    update_callback_(timestep);

    draw_callback_();

    graphics_system_->EndFrame();
  }
}

Engine::~Engine() { graphics_system_->Shutdown(); }

void Engine::Stop() { running_ = false; }

void Engine::AddScene(std::unique_ptr<Scene> scene) {
  auto it = scenes_.find(scene->GetName());
  EASSERT_MSG(it == scenes_.end(), "Scene Added Already");
  scenes_.emplace(scene->name_, std::move(scene));
}

void Engine::LoadScene(const std::string& name) {
  auto it = scenes_.find(name);
  EASSERT_MSG(it != scenes_.end(), "Scene Not Found");
  active_scene_ = it->second.get();
}
