#include "graphics_system.h"

#include <entt/entity/registry.hpp>

#include "../../renderer/renderer.h"
#include "engine/ecs/component/renderer_components.h"
#include "engine/ecs/component/transform.h"
#include "engine/renderer/material.h"
#include "engine/renderer/renderer_types.h"
#include "engine/resource/material_manager.h"
#include "engine/scene.h"
#include "engine/util/profiler.h"

namespace engine {

using namespace component;
using namespace gfx;

void GraphicsSystem::Init() {
  PROFILE_FUNCTION();
  renderer_.Init();
}
void GraphicsSystem::Shutdown() { renderer_.Shutdown(); }

void GraphicsSystem::StartFrame(Scene& scene) {
  PROFILE_FUNCTION();
  renderer_.StartFrame(scene.GetViewInfo());
}

void GraphicsSystem::InitScene(Scene& scene) {}

GraphicsSystem::GraphicsSystem(gfx::Renderer& renderer, MaterialManager& material_manager)
    : renderer_(renderer), material_manager_(material_manager) {}

void update_model_matrices(entt::registry& registry) {
  PROFILE_FUNCTION();
  auto model_group = registry.view<Transform, ModelMatrix>();
  std::for_each(std::execution::par_unseq, model_group.begin(), model_group.end(),
                [&model_group](auto entity) {
                  auto [transform, model] = model_group.get<Transform, ModelMatrix>(entity);
                  if (transform.IsDirty()) {
                    model.matrix = transform.CalculateModel();
                    transform.SetModel();
                  }
                });
}

void GraphicsSystem::DrawOpaque(entt::registry& registry) {
  PROFILE_FUNCTION();
  // auto par_group = registry.group<Transform>(entt::get<Parent>);
  // for (auto entity : par_group) {
  //   auto [world_transform, parent] = par_group.get<Transform, Parent>(entity);
  // }

  // TODO(tony): model matrix update in physics system along with scene graph
  update_model_matrices(registry);
  auto draw_cmd_group = registry.view<ModelMatrix, MeshMaterial>();
  draw_cmd_group.each([this](auto& model_matrix, auto& mesh_material) {
    renderer_.SubmitDrawCommand(model_matrix.matrix, mesh_material.mesh_handle,
                                mesh_material.material_handle);
  });

  renderer_.RenderOpaqueObjects();
}

void GraphicsSystem::EndFrame() {
  PROFILE_FUNCTION();
  renderer_.EndFrame();
}
}  // namespace engine
