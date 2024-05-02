#include "window_system.h"

#include "engine/application/engine.h"
#include "engine/application/event.h"
#include "engine/renderer/renderer.h"

void framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height) {
  auto* engine = static_cast<Engine*>(glfwGetWindowUserPointer(glfw_window));
  engine->window_system_->framebuffer_height_ = height;
  engine->window_system_->framebuffer_width_ = width;
  gfx::Renderer::SetFrameBufferSize(width, height);
}

void window_size_callback(GLFWwindow* glfw_window, int width, int height) {
  auto* engine = static_cast<Engine*>(glfwGetWindowUserPointer(glfw_window));
  Event e;
  e.type = Event::WindowResize;
  e.size.x = width;
  e.size.y = height;
  engine->OnEvent(e);
}

void WindowSystem::CenterCursor() {
  int width;
  int height;
  glfwGetWindowSize(glfw_window_, &width, &height);
  glfwSetCursorPos(glfw_window_, static_cast<float>(width) / 2.0f,
                   static_cast<float>(height) / 2.0f);
  // glfwSetCursorPos(glfw_window_, framebuffer_width_ / 2.0f, framebuffer_height_ / 2.0f);
}

void WindowSystem::SetCursorVisible(bool state) {
  glfwSetInputMode(glfw_window_, GLFW_CURSOR, state ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

bool WindowSystem::GetCursorVisible() const {
  return glfwGetInputMode(glfw_window_, GLFW_CURSOR) != 0;
}

glm::vec2 WindowSystem::GetWindowDimensions() const {
  int w;
  int h;
  glfwGetWindowSize(glfw_window_, &w, &h);
  return {w, h};
}

void WindowSystem::Init(Engine* engine) {
  glfwSetErrorCallback([](int error, const char* description) {
    spdlog::critical("GFLW error {}: {}\n", error, description);
  });

  EASSERT(glfwInit() != false);
  // if (!glfwInit()) {
  //   spdlog::critical("GLFW initialization failed");
  // }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  // const char* glsl_version = "#version 410";

  //  const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  //  m_windowWidth = videoMode->width / 2;
  //  m_windowHeight = videoMode->height / 2;

  // glfw_window_ = glfwCreateWindow(800, 600, "OpenGL Modern", nullptr, nullptr);
  glfw_window_ = glfwCreateWindow(1600, 900, "OpenGL Modern", nullptr, nullptr);

  if (!glfw_window_) {
    spdlog::critical("Failed to create GLFW window");
    glfwTerminate();
  }

  glfwMakeContextCurrent(glfw_window_);

  // one window for now, if multiple windows ever, (probably not, then make this function public to
  // set the active window class)
  glfwSetWindowUserPointer(glfw_window_, static_cast<void*>(engine));

  glfwSetFramebufferSizeCallback(glfw_window_, framebuffer_size_callback);
  glfwSetWindowSizeCallback(glfw_window_, window_size_callback);

  int curr_framebuffer_width;
  int curr_framebuffer_height;
  glfwGetFramebufferSize(glfw_window_, &curr_framebuffer_width, &curr_framebuffer_height);
  framebuffer_width_ = curr_framebuffer_width;
  framebuffer_height_ = curr_framebuffer_height;

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    spdlog::critical("Failed to initialize glew");
    glfwTerminate();
  }

  // query all available extensions
  std::vector<std::string> opengl_extensions;
  opengl_extensions.clear();
  GLint extension_count = 0;
  glGetIntegerv(GL_NUM_EXTENSIONS, &extension_count);
  for (int i = 0; i < extension_count; i++) {
    opengl_extensions.emplace_back(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)));
  }
  std::cout << opengl_extensions.size() << "\n";
}

void WindowSystem::SwapBuffers() { glfwSwapBuffers(glfw_window_); }

void WindowSystem::SetVsync(bool state) {
  is_vsync_ = state;
  glfwSwapInterval(state);
};

void WindowSystem::Shutdown() { glfwSetWindowShouldClose(glfw_window_, true); }

bool WindowSystem::ShouldClose() { return glfwWindowShouldClose(glfw_window_); }
