#pragma once

#include "engine.h"

struct Timestep;

class Application {
 public:
  inline static void SetInitCallback(void (*init_callback)()) { init_callback_ = init_callback; }
  inline static void SetUpdateCallback(void (*update_callback)(Timestep)) {
    update_callback_ = update_callback;
  }
  inline static void SetDrawCallback(void (*draw_callback)()) { draw_callback_ = draw_callback; }

  static void Start();
  static void Quit();
  static void Shutdown();

 private:
  inline static Engine* engine_{nullptr};

  inline static void (*init_callback_)(){nullptr};
  inline static void (*update_callback_)(Timestep){nullptr};
  inline static void (*draw_callback_)(){nullptr};
};
