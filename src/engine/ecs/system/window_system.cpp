#include "window_system.h"

void WindowSystem::Init() {
  // TODO(tony): opengl error callback
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

  glfw_window_ = glfwCreateWindow(800, 600, "OpenGL Modern", nullptr, nullptr);

  if (!glfw_window_) {
    spdlog::critical("Failed to create GLFW window");
    glfwTerminate();
  }

  glfwMakeContextCurrent(glfw_window_);

  // one window for now, if multiple windows ever, (probably not, then make this function public to
  // set the active window class)
  glfwSetWindowUserPointer(glfw_window_, static_cast<void*>(this));

  glfwSetFramebufferSizeCallback(glfw_window_, [](GLFWwindow* glfw_window, int width, int height) {
    WindowSystem* this_window;
    this_window = static_cast<WindowSystem*>(glfwGetWindowUserPointer(glfw_window));
    this_window->framebuffer_height_ = height;
    this_window->framebuffer_width_ = width;
  });

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
