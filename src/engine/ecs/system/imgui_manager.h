#pragma once

#include "engine/core/base.h"
namespace engine {

namespace gfx {
struct RendererStats;
}

struct ImGuiManager {
 public:
  void Init(GLFWwindow* window);
  void Shutdown();
  void StartFrame();
  void EndFrame();
  void FramerateSubMenu(Timestep timestep);

  bool debug_menu_enabled{true};
  bool enabled{true};
};
}  // namespace engine
