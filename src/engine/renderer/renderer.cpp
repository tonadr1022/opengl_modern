#include "renderer.h"

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <spdlog/spdlog.h>

namespace renderer {

GLFWwindow* InitContext() {
  glfwSetErrorCallback([](int error, const char* description) {
    spdlog::critical("GFLW error {}: {}\n", error, description);
  });
  if (!glfwInit()) {
    // spdlog::critical("glfw ")
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  const char* glsl_version = "#version 410";

  //  const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  //  m_windowWidth = videoMode->width / 2;
  //  m_windowHeight = videoMode->height / 2;

  GLFWwindow* window = glfwCreateWindow(1600, 900, "OpenGL Renderer", nullptr, nullptr);
  // m_window = glfwCreateWindow(800, 600, "OpenGL Renderer", nullptr, nullptr);

  if (!window) {
    spdlog::critical("Failed to create GLFW window");
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    spdlog::critical("Failed to initialize Glad");
  }

  return window;
}
}  // namespace renderer
