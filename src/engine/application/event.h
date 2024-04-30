#pragma once

#include "engine/application/input.h"
#include "key_codes.h"

// Heavily based on SFML event struct:
// https://github.com/SFML/SFML/blob/b447d41c6e07a4b3de55848737998b41659f6e78/include/SFML/Window/Event.hpp

struct Event {
  struct KeyEvent {
    KeyCode code;
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

  struct MouseScroll {
    float offset;
  };

  enum Type {
    WindowClose,
    WindowResize,
    FrameBufferResize,
    WindowFocused,
    WindowLostFocus,
    WindowMoved,
    KeyPressed = GLFW_PRESS,
    KeyReleased = GLFW_REPEAT,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled
  };

  Type type{};

  union {
    KeyEvent key;
    WindowSize size;
    MouseScroll scroll;
  };
};
