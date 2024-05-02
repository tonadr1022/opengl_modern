#include "input.h"

#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>

#include "engine/application/engine.h"
#include "engine/application/event.h"
#include "engine/application/mouse_codes.h"

namespace engine {

void Input::Update() {
  // key states decause to down or up after each frame
  for (auto& mouse_button_state : mouse_button_states_) {
    if (mouse_button_state & Pressed) mouse_button_state = Down;
    if (mouse_button_state & Released) mouse_button_state = Up;
  }

  for (auto& key_state : key_states_) {
    if (key_state & Pressed) key_state = Down;
    if (key_state & Released) key_state = Up;
  }

  double x;
  double y;
  glfwGetCursorPos(glfw_window_, &x, &y);
  prev_cursor_pos_ = cursor_pos_;
  cursor_pos_ = {x, y};
  // cursor_offset_ = {0, 0};

  glfwPollEvents();
}

bool Input::IsKeyDown(KeyCode key) { return key_states_[static_cast<uint16_t>(key)] & Down; }

bool Input::IsKeyUp(KeyCode key) { return key_states_[static_cast<uint16_t>(key)] & Down; }

bool Input::IsKeyPressed(KeyCode key) { return key_states_[static_cast<uint16_t>(key)] == Pressed; }

bool Input::IsKeyReleased(KeyCode key) {
  return key_states_[static_cast<uint16_t>(key)] == Released;
}

bool Input::IsMouseDown(MouseButton key) { return mouse_button_states_[key] & Down; }

bool Input::IsMouseUp(MouseButton key) { return mouse_button_states_[key] & Up; }

bool Input::IsMousePressed(MouseButton key) { return mouse_button_states_[key] == Pressed; }

bool Input::IsMouseReleased(MouseButton key) { return mouse_button_states_[key] == Released; }

void Input::init_glfw_input_callbacks(GLFWwindow* window) {
  glfw_window_ = window;
  EASSERT(window != nullptr);
  glfwSetKeyCallback(window, keypress_cb);
  glfwSetCursorPosCallback(window, mouse_pos_cb);
  glfwSetMouseButtonCallback(window, mouse_button_cb);
  glfwSetScrollCallback(window, mouse_scroll_cb);
  glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
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
  e.type = static_cast<EventType>(action);
  e.key.action = static_cast<InputAction>(action);
  e.key.code = static_cast<KeyCode>(key);
  e.key.alt = mods & GLFW_MOD_ALT;
  e.key.control = mods & GLFW_MOD_CONTROL;
  e.key.shift = mods & GLFW_MOD_SHIFT;
  e.key.system = mods & GLFW_MOD_SUPER;

  if (static_cast<InputAction>(action) == InputAction::Press) {
    key_states_[key] = Pressed;
  } else if (static_cast<InputAction>(action) == InputAction::Release) {
    key_states_[key] = Released;
  } else if (static_cast<InputAction>(action) == InputAction::Repeat) {
    key_states_[key] = Repeat;
  }
  auto* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->OnEvent(e);
}

void Input::mouse_pos_cb(GLFWwindow* window, double xpos, double ypos) {
  ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
  // static bool first_mouse = true;
  // if (first_mouse) {
  //   cursor_offset_.x = static_cast<float>(xpos);
  //   cursor_offset_.y = static_cast<float>(ypos);
  //   first_mouse = false;
  // }
  //
  // cursor_pos_.x = static_cast<float>(xpos);
  // cursor_pos_.y = static_cast<float>(ypos);
  //
  // cursor_offset_.x = static_cast<float>(xpos) - prev_cursor_offset_.x;
  // cursor_offset_.y = prev_cursor_offset_.y - static_cast<float>(ypos);
  // prev_cursor_offset_ = glm::vec2(xpos, ypos);
  //
  Event e{.type = EventType::MouseMoved};
  e.mouse_pos.x = xpos;
  e.mouse_pos.y = ypos;
  auto* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->OnEvent(e);
}

void Input::mouse_scroll_cb(GLFWwindow* window, double xoffset, double yoffset) {
  ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
  Event e{.type = EventType::MouseScrolled};
  e.scroll.offset = yoffset;
  auto* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->OnEvent(e);
}

void Input::mouse_button_cb(GLFWwindow* window, int button, int action, int mods) {
  ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
  Event e;
  if (action == GLFW_PRESS) {
    e.type = EventType::MouseButtonPressed;
    e.mouse.action = InputAction::Press;
    mouse_button_states_[button] = Pressed;
  } else if (action == GLFW_RELEASE) {
    e.type = EventType::MouseButtonReleased;
    e.mouse.action = InputAction::Release;
    mouse_button_states_[button] = Released;
  }
  e.mouse.code = static_cast<MouseCode>(button);
  e.mouse.alt = mods & GLFW_MOD_ALT;
  e.mouse.control = mods & GLFW_MOD_CONTROL;
  e.mouse.shift = mods & GLFW_MOD_SHIFT;
  e.mouse.system = mods & GLFW_MOD_SUPER;
  auto* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->OnEvent(e);
}

}  // namespace engine
