#pragma once

#include <memory>

#include "ecs/system/graphics_system.h"

struct Timestep;

class Engine {
 public:
  ~Engine();
  void Run();

 private:
  friend class Application;
  Engine();
  void Stop();
  bool running_{false};
  void (*update_callback_)(Timestep){nullptr};
  void (*draw_callback_)(){nullptr};
  void (*init_callback_)(){nullptr};

  std::unique_ptr<GraphicsSystem> graphics_system_{nullptr};
};
