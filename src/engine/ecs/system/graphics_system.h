#pragma once

#include <entt/entity/fwd.hpp>

namespace engine {

namespace gfx {
struct RenderViewInfo;
class Renderer;
}  // namespace gfx

class Scene;

struct GraphicsSystem {
 public:
  void Init(gfx::Renderer* renderer_);
  void Shutdown();
  void StartFrame(Scene& scene);
  void InitScene(Scene& scene);

  void UpdateTransforms(entt::registry& registry);
  void DrawOpaque(entt::registry& registry);
  void EndFrame();

 private:
  gfx::Renderer* renderer_{nullptr};
  void submit_cmds(entt::registry& registry);
  void submit_dynamic_cmds(entt::registry& registry);
};
}  // namespace engine
