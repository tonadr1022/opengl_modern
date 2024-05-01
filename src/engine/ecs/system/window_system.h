#pragma once

struct GLFWwindow;
class Engine;

struct WindowSystem {
  void Init(Engine* engine);
  void SwapBuffers();
  void SetVsync(bool state);
  void Shutdown();
  void SetCursorVisible(bool state);
  void CenterCursor();
  bool ShouldClose();
  [[nodiscard]] bool GetVsync() const { return is_vsync_; };
  [[nodiscard]] GLFWwindow* GetContext() const { return glfw_window_; }
  [[nodiscard]] bool GetCursorVisible() const;

  friend void framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height);

 private:
  bool is_vsync_;
  GLFWwindow* glfw_window_;
  uint32_t framebuffer_width_;
  uint32_t framebuffer_height_;
};
