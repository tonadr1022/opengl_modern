#pragma once

#include <entt/entity/fwd.hpp>

namespace engine {

class MaterialManager;
namespace gfx {
struct RenderViewInfo;
class Renderer;
}  // namespace gfx

class Scene;

struct GraphicsManager {
 public:
  void Init();
  void Shutdown();
  void OnImGuiRender();
  void StartFrame(Scene& scene);
  void ResetOnSceneChange(Scene& scene);
  void UpdateTransforms(entt::registry& registry);
  void DrawOpaque(Scene& scene);
  void EndFrame();
};
}  // namespace engine
