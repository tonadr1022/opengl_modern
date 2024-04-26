#pragma once

#include "engine/pch.h"

struct Timestep;

struct ImGuiSystem {
 public:
  void Init(GLFWwindow* window);
  void Shutdown();
  void StartFrame();
  void EndFrame();
  void DebugMenu(Timestep timestep);

  bool debug_menu_enabled{true};
  bool enabled{true};
};
