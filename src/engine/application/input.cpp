#include "input.h"

#include <imgui_impl_glfw.h>

#include "engine/application/engine.h"
#include "engine/application/event.h"

void Input::Update() {
  for (auto& mouse_button_state : mouse_button_states_) {
    if (mouse_button_state & Pressed) mouse_button_state = Down;
    if (mouse_button_state & Released) mouse_button_state = Up;
  }

  for (auto& key_state : key_states_) {
    if (key_state & Pressed) key_state = Down;
    if (key_state & Released) key_state = Up;
  }

  glfwPollEvents();
}

bool Input::IsKeyDown(KeyCode key) { return key_states_[static_cast<uint16_t>(key)] == Down; }

bool Input::IsKeyUp(KeyCode key) { return key_states_[static_cast<uint16_t>(key)] == Down; }

bool Input::IsKeyPressed(KeyCode key) { return key_states_[static_cast<uint16_t>(key)] == Pressed; }

bool Input::IsKeyReleased(KeyCode key) {
  return key_states_[static_cast<uint16_t>(key)] == Released;
}

bool Input::IsMouseDown(MouseButton key) { return mouse_button_states_[key] == Down; }

bool Input::IsMouseUp(MouseButton key) { return mouse_button_states_[key] == Up; }

bool Input::IsMousePressed(MouseButton key) { return mouse_button_states_[key] == Pressed; }

bool Input::IsMouseReleased(MouseButton key) { return mouse_button_states_[key] == Released; }

void Input::init_glfw_input_callbacks(GLFWwindow* window) {
  EASSERT(window != nullptr);
  glfwSetKeyCallback(window, keypress_cb);
  glfwSetCursorPosCallback(window, mouse_pos_cb);
  glfwSetMouseButtonCallback(window, mouse_button_cb);
  glfwSetScrollCallback(window, mouse_scroll_cb);
  // glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
}

// void Input::SetCursorVisible(bool state) {
//   int mode = state ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
//   glfwSetInputMode(window_, GLFW_CURSOR, mode);
// }

// bool Input::GetCursorVisible() { return glfwGetInputMode(window_, GLFW_CURSOR) != 0; }

void Input::keypress_cb(GLFWwindow* window, int key, int scancode, int action, int mods) {
  ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
  //  ImGuiIO& io = ImGui::GetIO();
  //  if (action == GLFW_PRESS) {
  //    io.KeysDown[key] = true;
  //  }
  //  if (action == GLFW_RELEASE) {
  //    io.KeysDown[key] = false;
  //  }

  //  io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] ||
  //  io.KeysDown[GLFW_KEY_RIGHT_CONTROL]; io.KeyShift =
  //  io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
  //  io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] ||
  //  io.KeysDown[GLFW_KEY_RIGHT_ALT]; io.KeySuper =
  //  io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

  Event e;
  e.type = static_cast<Event::EventType>(action);
  e.key.code = static_cast<KeyCode>(key);
  e.key.alt = mods & GLFW_MOD_ALT;
  e.key.control = mods & GLFW_MOD_CONTROL;
  e.key.shift = mods & GLFW_MOD_SHIFT;
  e.key.system = mods & GLFW_MOD_SUPER;

  auto* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));

  if (static_cast<InputAction>(action) == InputAction::Press) {
    key_states_[key] = Pressed;
  } else if (static_cast<InputAction>(action) == InputAction::Release) {
    key_states_[key] = Released;
  }
}

void Input::mouse_pos_cb(GLFWwindow* window, double xpos, double ypos) {
  ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
  // static double last_x;
  // static double last_y;
  // double x_offset = xpos - last_x;
  // double y_offset = ypos - last_y;
  // last_x = xpos;
  // last_y = ypos;
  // mouse_screen_offset_ = {x_offset, y_offset};
  // mouse_screen_pos_ = {xpos, ypos};
  // mouse_moved_ = true;
}

void Input::mouse_scroll_cb(GLFWwindow* window, double xoffset, double yoffset) {
  ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
  Event e{.type = Event::MouseScrolled};
  e.scroll.offset = yoffset;
}

void Input::mouse_button_cb(GLFWwindow* window, int button, int action, int mods) {
  ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
  Event e{.type = Event::MouseButtonPressed};
  if (action == GLFW_PRESS) {
    mouse_button_states_[button] = Pressed;
  } else if (action == GLFW_RELEASE) {
    mouse_button_states_[button] = Released;
  }
}

// void Input::SetCursorPos(float x, float y) { glfwSetCursorPos(window_, x, y); }

// void Input::CenterCursor() {
//   int width;
//   int height;
//   glfwGetWindowSize(window_, &width, &height);
//   glfwSetCursorPos(window_, static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f);
// }

// const glm::vec2& Input::GetMousePosOffset() { return mouse_screen_offset_; }
// const glm::vec2& Input::GetMousePosition() { return mouse_screen_pos_; }

// bool Input::MouseMoved() { return mouse_moved_; }
