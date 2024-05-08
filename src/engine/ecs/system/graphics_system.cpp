#include "graphics_system.h"

#include <entt/entity/registry.hpp>

#include "../../renderer/renderer.h"
#include "engine/ecs/component/renderer_components.h"
#include "engine/ecs/component/transform.h"
#include "engine/resource/material_manager.h"
#include "engine/scene.h"

namespace engine {

using namespace component;
using namespace gfx;

void GraphicsSystem::Init() { renderer_.Init(); }
void GraphicsSystem::Shutdown() { renderer_.Shutdown(); }

void GraphicsSystem::StartFrame(Scene& scene) { renderer_.StartFrame(scene.GetViewInfo()); }

void GraphicsSystem::InitScene(Scene& scene) { renderer_.Reset(); }

GraphicsSystem::GraphicsSystem(gfx::Renderer& renderer, MaterialManager& material_manager)
    : renderer_(renderer), material_manager_(material_manager) {}

void update_model_matrices(entt::registry& registry) {
  ZoneScopedN("update model matrices");
  auto model_group = registry.view<Transform, ModelMatrix>();
  int i = 0;
  std::for_each(model_group.begin(), model_group.end(), [&i, model_group](auto entity) {
    auto [transform, model] = model_group.get<Transform, ModelMatrix>(entity);
    i++;
    if (transform.IsDirty()) {
      model.matrix = transform.CalculateModel();
      transform.SetModel();
    }
  });
}

void GraphicsSystem::DrawOpaque(entt::registry& registry) {
  // auto par_group = registry.group<Transform>(entt::get<Parent>);
  // for (auto entity : par_group) {
  //   auto [world_transform, parent] = par_group.get<Transform, Parent>(entity);
  // }

  // TODO(tony): model matrix update in physics system along with scene graph
  update_model_matrices(registry);
  {
    ZoneScopedN("submit cmds");
    auto draw_cmd_group = registry.view<ModelMatrix, MeshMaterial>();
    draw_cmd_group.each([this](auto& model_matrix, auto& mesh_material) {
      renderer_.SubmitDrawCommand(model_matrix.matrix, mesh_material.mesh_handle,
                                  mesh_material.material_handle);
    });
  }

  renderer_.RenderOpaqueObjects();
}

void GraphicsSystem::EndFrame() { renderer_.EndFrame(); }
}  // namespace engine
