#pragma once

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

#include "engine/application/key_codes.h"
#include "engine/application/mouse_codes.h"

struct GLFWwindow;

namespace engine {

using MouseButton = int;

enum class InputAction {
  // from glfw
  Press = 1,
  Release = 0,
  Repeat = 2,
};

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

  static void Update();
  static bool IsKeyDown(KeyCode key);
  static bool IsKeyUp(KeyCode key);
  static bool IsKeyPressed(KeyCode key);
  static bool IsKeyReleased(KeyCode key);
  static bool IsMouseDown(MouseButton key);
  static bool IsMouseUp(MouseButton key);
  static bool IsMousePressed(MouseButton key);
  static bool IsMouseReleased(MouseButton key);
  static bool MouseMoved();
  static bool IsMouseScroll();

  inline static double GetScrollOffset() { return scroll_offset_; };
  inline static glm::vec2 GetCursorOffset() { return cursor_pos_ - prev_cursor_pos_; };

 private:
  friend class Engine;
  inline static GLFWwindow* glfw_window_{nullptr};
  static void init_glfw_input_callbacks(GLFWwindow* window);
  // inline static glm::vec2 cursor_offset_{0, 0};
  // inline static glm::vec2 prev_cursor_offset_{0, 0};
  inline static glm::vec2 cursor_pos_{0, 0};
  inline static glm::vec2 prev_cursor_pos_{0, 0};
  inline static double scroll_offset_{0};

  // static inline glm::vec2 mouse_screen_pos_;
  // static inline glm::vec2 mouse_screen_offset_;
  // static inline glm::vec2 prev_mouse_screen_pos_;
  // static inline glm::vec2 mouse_scroll_offset_;
  // static inline bool mouse_moved_;

  static void keypress_cb(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void mouse_pos_cb(GLFWwindow* window, double xpos, double ypos);
  static void mouse_scroll_cb(GLFWwindow* window, double xoffset, double yoffset);
  static void mouse_button_cb(GLFWwindow* window, int button, int action, int mods);

  // from gflw
  static constexpr int ButtonCount = 348;
  static constexpr int MouseButtonStates = 7;
  static inline KeyState key_states_[ButtonCount] = {static_cast<KeyState>(0)};
  static inline KeyState mouse_button_states_[MouseButtonStates] = {static_cast<KeyState>(0)};
};
}  // namespace engine
