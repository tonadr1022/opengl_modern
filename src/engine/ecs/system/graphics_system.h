#pragma once

#include <entt/entity/fwd.hpp>
namespace engine {

namespace gfx {
struct CameraMatrices;
}
class Scene;

struct GraphicsSystem {
 public:
  void Init();
  void Shutdown();
  void StartFrame(const gfx::CameraMatrices& camera_matrices);
  void UpdateTransforms(entt::registry& registry);
  void DrawOpaque(entt::registry& registry);
  void EndFrame();
};
}  // namespace engine
