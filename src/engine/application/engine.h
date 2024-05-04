#pragma once

#include "engine/pch.h"

namespace engine {

struct Timestep;
class Scene;
struct GraphicsSystem;
struct WindowSystem;
struct ImGuiSystem;
struct Event;

class Engine {
 public:
  ~Engine();
  Engine();
  void Run();
  void Stop();
  void AddScene(std::unique_ptr<Scene> scene);
  void LoadScene(const std::string& name);

  WindowSystem* window_system_{nullptr};

  static Engine& Get();

 private:
  static Engine* instance_;
  friend class Input;
  friend void framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height);
  friend void window_size_callback(GLFWwindow* glfw_window, int width, int height);

  void OnEvent(const Event& e);
  void Shutdown();
  bool draw_imgui_{true};
  // static Engine* instance_;
  // TODO(tony): serialize scenes instead of this?
  std::unordered_map<std::string, std::unique_ptr<Scene>> scenes_;
  GraphicsSystem* graphics_system_{nullptr};
  ImGuiSystem* imgui_system_{nullptr};

  Scene* active_scene_{nullptr};
  bool running_{false};
  void ImGuiSystemPerFrame(Timestep timestep);
};

}  // namespace engine
