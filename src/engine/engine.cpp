#include "engine.h"

#include <iostream>
#include <memory>

#include "e_assert.h"
#include "ecs/system/graphics_system.h"
#include "timestep.h"
#include "util/timer.h"

Engine::Engine() {
  graphics_system_ = std::make_unique<GraphicsSystem>();
  graphics_system_->Init();

  running_ = true;
}

void Engine::Run() {
  EASSERT_MSG(update_callback_ != nullptr, "update callback");
  EASSERT_MSG(draw_callback_ != nullptr, "draw callback");
  EASSERT_MSG(init_callback_ != nullptr, "init callback");

  Timestep timestep;
  Timer timer;
  timer.Start();
  double last_time = timer.GetElapsedSeconds();
  while (running_) {
    std::cout << "running\n";
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
