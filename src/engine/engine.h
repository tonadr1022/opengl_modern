#pragma once

#include "ecs/system/graphics_system.h"
#include "pch.h"

struct Timestep;
class Scene;

class Engine {
 public:
  ~Engine();

 private:
  friend class Application;
  friend class Input;
  Engine();

  void Run();
  void Stop();
  void AddScene(std::unique_ptr<Scene> scene);
  void LoadScene(const std::string& name);

  std::unordered_map<std::string, std::unique_ptr<Scene>> scenes_;

  Scene* active_scene_{nullptr};

  bool running_{false};
  void (*update_callback_)(Timestep){nullptr};
  void (*draw_callback_)(){nullptr};
  void (*init_callback_)(){nullptr};
  void (*key_event_callback_)(int key, int action, int mods);

  std::unique_ptr<GraphicsSystem> graphics_system_{nullptr};
};
