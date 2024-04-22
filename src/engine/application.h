#pragma once
#include <functional>
#include <utility>

#include "timestep.h"

class Application {
 public:
  inline static void SetInitCallback(std::function<void()> function) {
    init_callback_ = std::move(function);
  }
  inline static void SetUpdateCallback(std::function<void(Timestep)> function) {
    update_callback_ = std::move(function);
  }
  inline static void SetDrawCallback(std::function<void()> function) {
    draw_callback_ = std::move(function);
  }

  static void Init();
  static void Run();
  static void Quit();
  static void Shutdown();

 private:
  inline static bool running_{false};
  inline static std::function<void()> init_callback_{nullptr};
  inline static std::function<void(Timestep)> update_callback_{nullptr};
  inline static std::function<void()> draw_callback_{nullptr};
};
