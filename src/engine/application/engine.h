#pragma once

#include "engine/application/input.h"
#include "engine/pch.h"

struct Timestep;
class Scene;

struct GraphicsSystem;
struct WindowSystem;
struct ImGuiSystem;

class Engine {
 public:
  ~Engine();
  Engine();
  void Run();
  void Stop();
  void AddScene(std::unique_ptr<Scene> scene);
  void LoadScene(const std::string& name);
  void OnKeyEvent(KeyEvent& e);
  void ImGuiSystemPerFrame(Timestep timestep);

  static Engine& Get();

 private:
  void Shutdown();
  static Engine* instance_;
  std::unordered_map<std::string, std::unique_ptr<Scene>> scenes_;
  GraphicsSystem* graphics_system_{nullptr};
  WindowSystem* window_system_{nullptr};
  ImGuiSystem* imgui_system_{nullptr};

  std::bitset<32> enabled_systems_;

  Scene* active_scene_{nullptr};
  bool running_{false};
};
