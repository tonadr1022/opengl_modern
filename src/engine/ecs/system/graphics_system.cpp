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

void GraphicsSystem::InitScene(Scene& scene) {
  renderer_.SetMaterials(material_manager_.GetAllMaterials());
}

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

void GraphicsSystem::submit_cmds(entt::registry& registry) {
  PROFILE_FUNCTION();
  // auto group = registry.group<Mesh>(entt::get<Mesh, ModelMatrix, Material>);
  auto group = registry.view<Mesh, ModelMatrix>();
  renderer_.SetBatchedObjectCount(group.size_hint());
  group.each([this](const auto& mesh, const auto& model) {
    renderer_.SubmitDrawCommand(model.matrix, mesh.handle, 0);
  });
}

void GraphicsSystem::submit_dynamic_cmds(entt::registry& registry) {
  // auto group = registry.group<Mesh>(entt::get<ModelMatrix, Material>);
  // renderer_->SetBatchedObjectCount(group.size());
  // group.each([](const auto& mesh, const auto& model, const auto& material) {
  //   renderer_->SubmitDrawCommand(model.matrix, mesh.handle, material.handle);
  // });
}

void GraphicsSystem::DrawOpaque(entt::registry& registry) {
  PROFILE_FUNCTION();
  // auto par_group = registry.group<Transform>(entt::get<Parent>);
  // for (auto entity : par_group) {
  //   auto [world_transform, parent] = par_group.get<Transform, Parent>(entity);
  // }

  // update model matrices
  update_model_matrices(registry);
  submit_cmds(registry);
  submit_dynamic_cmds(registry);

  renderer_.RenderOpaqueObjects();
}

void GraphicsSystem::EndFrame() {
  PROFILE_FUNCTION();
  renderer_.EndFrame();
}
}  // namespace engine
