#include "application.h"

#include <iostream>

#include "e_assert.h"
#include "util/timer.h"

void Application::Init() {
  EASSERT_MSG(init_callback_ != nullptr, "Init callback");
  init_callback_();
}

void Application::Run() {
  EASSERT_MSG(draw_callback_ != nullptr, "Draw Callback");
  EASSERT_MSG(update_callback_ != nullptr, "Update Callback");

  running_ = true;

  Timestep timestep;
  Timer timer;
  timer.Start();
  double last_time = timer.GetElapsedSeconds();
  while (running_) {
    auto current_time = timer.GetElapsedSeconds();
    timestep.delta_time = current_time - last_time;
    last_time = current_time;
    timestep.elapsed_time = current_time;

    update_callback_(timestep);
    draw_callback_();
  }
}

void Application::Shutdown() { std::cout << "Shuttingdown\n"; }

void Application::Quit() { running_ = false; }
