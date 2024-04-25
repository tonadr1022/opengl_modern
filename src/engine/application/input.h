#pragma once

#include "../pch.h"

constexpr int ButtonCount = GLFW_KEY_LAST;
constexpr int MouseButtonStates = GLFW_MOUSE_BUTTON_LAST;

using Key = int;
using MouseButton = int;

class Input {
 public:
  enum KeyState : uint8_t {
    Down = 0b00001,
    // pressed only happens on initial down
    Pressed = 0b00011,
    Up = 0b00100,
    // release only happens on initial up
    Released = 0b01100,
    Repeat = 0b10001
  };

  static void Initialize(GLFWwindow* window);
  static void Update();
  static bool IsKeyDown(Key key);
  static bool IsKeyUp(Key key);
  static bool IsKeyPressed(Key key);
  static bool IsKeyReleased(Key key);
  static bool IsMouseDown(MouseButton key);
  static bool IsMouseUp(MouseButton key);
  static bool IsMousePressed(MouseButton key);
  static bool IsMouseReleased(MouseButton key);
  static void SetCursorPos(float x, float y);
  static void CenterCursor();
  static const glm::vec2& GetMousePosOffset();
  static const glm::vec2& GetMousePosition();
  static bool MouseMoved();

  static void SetCursorVisible(bool state);
  static bool GetCursorVisible();

  static inline GLFWwindow* window_;

 private:
  friend class Engine;
  static void init_glfw_input_callbacks(GLFWwindow* window);
  static inline glm::vec2 mouse_screen_pos_;
  static inline glm::vec2 mouse_screen_offset_;
  static inline glm::vec2 prev_mouse_screen_pos_;
  static inline glm::vec2 mouse_scroll_offset_;
  static inline bool mouse_moved_;

  static void keypress_cb(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void mouse_pos_cb(GLFWwindow* window, double xpos, double ypos);
  static void mouse_scroll_cb(GLFWwindow* window, double xoffset, double yoffset);
  static void mouse_button_cb(GLFWwindow* window, int button, int action, int mods);

  static inline KeyState key_states_[ButtonCount] = {static_cast<KeyState>(0)};
  static inline KeyState mouse_button_states_[MouseButtonStates] = {static_cast<KeyState>(0)};
};
