#pragma once

#include <cstdint>

struct GLFWwindow;
namespace engine {

class Engine;
struct WindowSystem {
  static WindowSystem& Get();
  void Init();
  void SwapBuffers();
  void SetVsync(bool state);
  void Shutdown();
  void SetCursorVisible(bool state);
  void CenterCursor();
  void SetUserPointer(void* ptr);
  bool ShouldClose();
  [[nodiscard]] glm::vec2 GetWindowDimensions() const;
  [[nodiscard]] bool GetVsync() const { return is_vsync_; };
  [[nodiscard]] GLFWwindow* GetContext() const { return glfw_window_; }
  [[nodiscard]] bool GetCursorVisible() const;

 private:
  static WindowSystem* instance_;
  bool is_vsync_;
  GLFWwindow* glfw_window_;
  uint32_t framebuffer_width_;
  uint32_t framebuffer_height_;
};
}  // namespace engine
