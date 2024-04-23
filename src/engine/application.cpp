#include "application.h"

#include <iostream>

#include "e_assert.h"

void Application::Start() {
  EASSERT_MSG(init_callback_ != nullptr, "Init callback");
  EASSERT_MSG(draw_callback_ != nullptr, "Draw Callback");
  EASSERT_MSG(update_callback_ != nullptr, "Update Callback");
  engine_ = new Engine();
  engine_->update_callback_ = update_callback_;
  engine_->draw_callback_ = draw_callback_;
  engine_->init_callback_ = init_callback_;

  engine_->Run();
}

void Application::Shutdown() { std::cout << "Shuttingdown\n"; }

void Application::Quit() {
  std::cout << "stopping engine\n";
  engine_->Stop();
  delete engine_;
}
