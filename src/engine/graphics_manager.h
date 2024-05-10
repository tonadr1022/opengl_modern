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
  void StartFrame(Scene& scene);
  void InitScene(Scene& scene);
  void UpdateTransforms(entt::registry& registry);
  void DrawOpaque(entt::registry& registry);
  void EndFrame();
};
}  // namespace engine
