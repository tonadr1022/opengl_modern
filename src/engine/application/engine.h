#pragma once

#include "engine/core/base.h"
namespace engine {

class Scene;
struct GraphicsManager;
struct WindowManager;
struct ImGuiManager;
struct Event;
class MaterialManager;
class MeshManager;
class ShaderManager;
namespace gfx {

class Renderer;
}

class Engine {
 public:
  ~Engine();
  Engine();
  void Run();
  void Stop();
  void LoadScene(std::unique_ptr<Scene> scene);

  static Engine& Get();

 private:
  friend class Scene;
  friend class Input;
  static Engine* instance_;

  void OnFrameBufferResize(uint32_t width, uint32_t height);
  friend void framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height);
  friend void window_size_callback(GLFWwindow* glfw_window, int width, int height);

  void OnEvent(const Event& e);
  void Shutdown();
  bool draw_imgui_{true};
  WindowManager* window_manager_{nullptr};
  GraphicsManager* graphics_system_{nullptr};
  ImGuiManager* imgui_system_{nullptr};
  MaterialManager* material_manager_{nullptr};
  ShaderManager* shader_manager_{nullptr};
  MeshManager* mesh_manager_{nullptr};
  gfx::Renderer* renderer_{nullptr};

  std::unique_ptr<Scene> active_scene_{nullptr};
  bool running_{false};
  void ImGuiSystemPerFrame(Timestep timestep);
};

}  // namespace engine
