#include "graphics_system.h"

#include <entt/entity/registry.hpp>

#include "../../renderer/renderer.h"
#include "engine/ecs/component/renderer_components.h"
#include "engine/ecs/component/transform.h"

namespace engine {

using namespace component;

void GraphicsSystem::Init() { gfx::Renderer::Init(); }
void GraphicsSystem::Shutdown() {}

void GraphicsSystem::StartFrame(const gfx::CameraMatrices& camera_matrices) {
  gfx::Renderer::StartFrame(camera_matrices);
}

void update_model_matrices(entt::registry& registry) {
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

void submit_cmds(entt::registry& registry) {
  auto group = registry.group<Mesh>(entt::get<ModelMatrix, Material>);
  gfx::Renderer::SetBatchedObjectCount(group.size());
  group.each([](const auto& mesh, const auto& model, const auto& material) {
    gfx::Renderer::SubmitDrawCommand(model.matrix, mesh.handle, material.handle);
  });
}

void GraphicsSystem::DrawOpaque(entt::registry& registry) {
  // auto par_group = registry.group<Transform>(entt::get<Parent>);
  // for (auto entity : par_group) {
  //   auto [world_transform, parent] = par_group.get<Transform, Parent>(entity);
  // }

  // update model matrices
  update_model_matrices(registry);
  submit_cmds(registry);

  gfx::Renderer::RenderOpaqueObjects();
}

void GraphicsSystem::EndFrame() { gfx::Renderer::EndFrame(); }
}  // namespace engine
