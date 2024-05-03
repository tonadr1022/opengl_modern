#include "graphics_system.h"

#include "../../renderer/renderer.h"
#include "engine/ecs/component/renderer_components.h"
#include "engine/ecs/component/transform.h"
#include "engine/scene.h"

namespace engine {

using namespace component;

void GraphicsSystem::Init() { gfx::Renderer::Init(); }
void GraphicsSystem::Shutdown() {}
void GraphicsSystem::StartFrame(Scene& scene) {
  gfx::Renderer::StartFrame(scene.view_matrix_, scene.projection_matrix_);
}

void GraphicsSystem::DrawOpaque(Scene& scene) {
  auto model_group = scene.registry.view<Transform, ModelMatrix>();
  std::for_each(std::execution::par_unseq, model_group.begin(), model_group.end(),
                [&model_group](auto entity) {
                  auto [transform, model] = model_group.get<Transform, ModelMatrix>(entity);
                  if (transform.IsDirty()) {
                    model.matrix = transform.CalculateModel();
                    transform.SetModel();
                  }
                });

  auto group = scene.registry.group<Mesh>(entt::get<ModelMatrix, Material>);
  gfx::Renderer::SetBatchedObjectCount(group.size());
  group.each([](const auto& mesh, const auto& model, const auto& material) {
    gfx::Renderer::SubmitDrawCommand(model.matrix, mesh.handle, material.handle);
  });

  gfx::Renderer::RenderOpaqueObjects();
}

void GraphicsSystem::EndFrame() { gfx::Renderer::EndFrame(); }
}  // namespace engine
