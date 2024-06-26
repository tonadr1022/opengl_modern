#pragma once

#include "engine/application/input.h"

// Heavily based on SFML event struct:
// https://github.com/SFML/SFML/blob/b447d41c6e07a4b3de55848737998b41659f6e78/include/SFML/Window/Event.hpp

namespace engine {

enum class EventType {
  kWindowClose,
  kWindowResize,
  kFrameBufferResize,
  kWindowFocused,
  kWindowLostFocus,
  kWindowMoved,
  kKeyPressed = 1,
  kKeyReleased = 2,
  kMouseButtonPressed,
  kMouseButtonReleased,
  kMouseMoved,
  kMouseScrolled
};

struct Event {
  struct KeyEvent {
    KeyCode code;
    InputAction action;
    bool alt;
    bool control;
    bool system;
    bool shift;
  };

  struct MouseEvent {
    MouseCode code;
    InputAction action;
    bool alt;
    bool control;
    bool system;
    bool shift;
  };

  struct WindowSize {
    uint32_t x;
    uint32_t y;
  };

  struct FrameBufferSize {
    uint32_t x;
    uint32_t y;
  };

  struct MouseScroll {
    float offset;
  };

  struct MouseMove {
    float x;
    float y;
  };

  EventType type{};

  union {
    KeyEvent key;
    WindowSize window_size;
    FrameBufferSize framebuffer_size;
    MouseScroll scroll;
    MouseMove mouse_pos;
    MouseEvent mouse;
  };
};
}  // namespace engine
