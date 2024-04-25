#pragma once

#include "../pch.h"
#include "engine.h"

struct Timestep;
class Scene;

class Application {
 public:
  inline static void SetInitCallback(void (*init_callback)()) { init_callback_ = init_callback; }
  inline static void SetUpdateCallback(void (*update_callback)(Timestep)) {
    update_callback_ = update_callback;
  }
  inline static void SetDrawCallback(void (*draw_callback)()) { draw_callback_ = draw_callback; }

  /** @brief GLFW key, action, and mods */
  inline static void SetKeyEventCallback(void (*key_event_callback)(int key, int action,
                                                                    int mods)) {
    key_event_callback_ = key_event_callback;
  }

  static void Init();
  /** @brief start the application after setting callback functions. */
  static void Run();
  /** @brief quit the application */
  static void Quit();
  /** @brief Must call at end of main loop. */
  static void Shutdown();

  static void LoadScene(const std::string& name);
  static void AddScene(std::unique_ptr<Scene> scene);
  static void GetScene(const std::string& name);

 private:
  friend class Input;
  inline static Engine* engine_{nullptr};
  inline static void (*key_event_callback_)(int key, int action, int mods){nullptr};

  inline static void (*init_callback_)(){nullptr};
  inline static void (*update_callback_)(Timestep){nullptr};
  inline static void (*draw_callback_)(){nullptr};
};
