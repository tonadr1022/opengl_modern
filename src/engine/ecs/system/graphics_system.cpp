#include "graphics_system.h"

#include <execution>

#include "../../renderer/renderer.h"
#include "engine/ecs/component/renderer_components.h"
#include "engine/ecs/component/transform.h"
#include "engine/scene.h"

void GraphicsSystem::Init() { gfx::renderer::Init(); }
void GraphicsSystem::Shutdown() {}
void GraphicsSystem::StartFrame() { gfx::renderer::StartFrame(); }

void GraphicsSystem::DrawOpaque(Scene& scene) {
  auto model_group = scene.registry.group<component::Transform, component::Model>();
  std::for_each(std::execution::par, model_group.begin(), model_group.end(),
                [&model_group](entt::entity entity) {
                  auto [transform, model] =
                      model_group.get<component::Transform, component::Model>(entity);
                  model.matrix = transform.CalculateModel();
                });
  auto group =
      scene.registry.group<component::Mesh>(entt::get<component::Model, component::Material>);

  gfx::renderer::SetBatchedObjectCount(group.size());

  // std::for_each(std::execution::par, group.begin(), group.end(), [&group](entt::entity entity) {
  //   auto [mesh, model, material] =
  //       group.get<component::Mesh, component::Model, component::Material>(entity);
  //   gfx::renderer::SubmitDrawCommand(model.matrix, mesh.handle, material.handle);
  // });
  group.each([](const auto& mesh, const auto& model, const auto& material) {
    gfx::renderer::SubmitDrawCommand(model.matrix, mesh.handle, material.handle);
  });

  gfx::renderer::RenderOpaqueObjects();
}

void GraphicsSystem::EndFrame() { gfx::renderer::EndFrame(); }
