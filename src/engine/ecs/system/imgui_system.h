#pragma once

namespace engine {

struct Timestep;
namespace gfx {
struct RendererStats;
}

struct ImGuiSystem {
 public:
  void Init(GLFWwindow* window);
  void Shutdown();
  void StartFrame();
  void EndFrame();
  void FramerateSubMenu(Timestep timestep);
  void RenderRendererStats(const gfx::RendererStats& stats);

  bool debug_menu_enabled{true};
  bool enabled{true};
};
}  // namespace engine
