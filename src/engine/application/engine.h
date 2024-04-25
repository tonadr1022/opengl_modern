#pragma once

#include "engine/application/input.h"
#include "engine/ecs/system/graphics_system.h"
#include "engine/pch.h"

struct Timestep;
class Scene;

class Engine {
 public:
  ~Engine();
  Engine();
  void Run();
  void Stop();
  void AddScene(std::unique_ptr<Scene> scene);
  void LoadScene(const std::string& name);
  void OnKeyEvent(KeyEvent& e);

  static Engine& Get();

 private:
  static Engine* instance_;
  std::unordered_map<std::string, std::unique_ptr<Scene>> scenes_;
  std::unique_ptr<GraphicsSystem> graphics_system_{nullptr};

  Scene* active_scene_{nullptr};
  bool running_{false};
};
