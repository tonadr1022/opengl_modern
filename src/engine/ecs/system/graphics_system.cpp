#include "graphics_system.h"

#include <execution>

#include "../../renderer/renderer.h"
#include "engine/ecs/component/renderer_components.h"
#include "engine/ecs/component/transform.h"
#include "engine/scene.h"

void GraphicsSystem::Init() { gfx::Renderer::Init(); }
void GraphicsSystem::Shutdown() {}
void GraphicsSystem::StartFrame(Scene& scene) {
  gfx::Renderer::StartFrame(scene.view_matrix_, scene.projection_matrix_);
}

void GraphicsSystem::DrawOpaque(Scene& scene) {
  auto model_group = scene.registry.group<component::Transform, component::ModelMatrix>();
  std::for_each(std::execution::par, model_group.begin(), model_group.end(),
                [&model_group](entt::entity entity) {
                  auto [transform, model] =
                      model_group.get<component::Transform, component::ModelMatrix>(entity);
                  model.matrix = transform.CalculateModel();
                  // model.matrix = glm::mat4(1);
                });

  auto group =
      scene.registry.group<component::Mesh>(entt::get<component::ModelMatrix, component::Material>);

  gfx::Renderer::SetBatchedObjectCount(group.size());

  // std::for_each(std::execution::par, group.begin(), group.end(), [&group](entt::entity entity) {
  //   auto [mesh, model, material] =
  //       group.get<component::Mesh, component::Model, component::Material>(entity);
  //   gfx::Renderer::SubmitDrawCommand(model.matrix, mesh.handle, material.handle);
  // });
  group.each([](const auto& mesh, const auto& model, const auto& material) {
    gfx::Renderer::SubmitDrawCommand(model.matrix, mesh.handle, material.handle);
  });

  gfx::Renderer::RenderOpaqueObjects();
}

void GraphicsSystem::EndFrame() { gfx::Renderer::EndFrame(); }
