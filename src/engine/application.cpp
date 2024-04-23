#include "application.h"

#include <spdlog/spdlog.h>

#include <iostream>

#include "scene.h"

void Application::Run() {
  engine_->update_callback_ = update_callback_;
  engine_->draw_callback_ = draw_callback_;
  engine_->init_callback_ = init_callback_;
  engine_->key_event_callback_ = key_event_callback_;
  if (engine_->active_scene_ == nullptr) {
    if (!engine_->scenes_.empty()) {
      spdlog::warn(
          "No active scene set even though scenes have been added, setting first scene active.");
      engine_->active_scene_ = engine_->scenes_.begin()->second.get();
    } else {
      engine_->scenes_.emplace("default", std::make_unique<Scene>("default"));
    }
  }
  engine_->Run();
}

void Application::Shutdown() { delete engine_; }

void Application::Quit() {
  std::cout << "stopping engine\n";
  engine_->Stop();
}

void Application::AddScene(std::unique_ptr<Scene> scene) { engine_->AddScene(std::move(scene)); }

void Application::LoadScene(const std::string& name) { engine_->LoadScene(name); }

void Application::Init() { engine_ = new Engine(); }
