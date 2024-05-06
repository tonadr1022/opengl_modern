#pragma once

#include <entt/entity/fwd.hpp>

namespace engine {

class MaterialManager;
namespace gfx {
struct RenderViewInfo;
class Renderer;
}  // namespace gfx

class Scene;

struct GraphicsSystem {
 public:
  GraphicsSystem(gfx::Renderer& renderer, MaterialManager& material_manager);
  void Init();
  void Shutdown();
  void StartFrame(Scene& scene);
  void InitScene(Scene& scene);

  void UpdateTransforms(entt::registry& registry);
  void DrawOpaque(entt::registry& registry);
  void EndFrame();

 private:
  gfx::Renderer& renderer_;
  void submit_cmds(entt::registry& registry);
  void submit_dynamic_cmds(entt::registry& registry);
  MaterialManager& material_manager_;
};
}  // namespace engine
