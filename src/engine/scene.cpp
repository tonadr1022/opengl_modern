#include "scene.h"

#include "engine/application/engine.h"
#include "engine/ecs/component/transform.h"
#include "engine/ecs/system/isystem.h"
#include "engine/timestep.h"

namespace engine {

Scene::Scene() {
  mesh_manager_ = Engine::Get().mesh_manager_;
  material_manager_ = Engine::Get().material_manager_;
  window_system_ = Engine::Get().window_system_;
  registry.ctx().emplace<MeshManager*>(Engine::Get().mesh_manager_);
  registry.ctx().emplace<MaterialManager*>(Engine::Get().material_manager_);
  registry.ctx().emplace<WindowSystem*>(Engine::Get().window_system_);
  auto camera_matrices_entity = registry.create();
  registry.emplace<entt::tag<entt::hashed_string{"view_info"}>>(camera_matrices_entity);
  registry.emplace<gfx::RenderViewInfo>(camera_matrices_entity);
}

void Scene::Init() {}

void Scene::OnEvent(const Event& e) {}

void Scene::OnUpdate(Timestep timestep) {}

void Scene::OnFixedUpdate(Timestep timestep) {}

void Scene::OnImGuiRender() {};

Scene::~Scene() { registry.clear(); }

void Scene::InitSystems(std::vector<ecs::ISystem*>& systems) {
  for (auto& sys : systems) sys->registry = &registry;
}
gfx::RenderViewInfo Scene::GetViewInfo() {
  auto view = registry.view<entt::tag<entt::hashed_string{"view_info"}>>();
  auto& view_info = registry.get<gfx::RenderViewInfo>(view.front());
  return view_info;
}

entt::entity Scene::MakeDynamicEntity() {
  auto entity = registry.create();
  registry.emplace<component::DynamicEntity>(entity);
  return entity;
}

entt::entity Scene::MakeStaticEntity() {
  auto entity = registry.create();
  registry.emplace<component::StaticEntity>(entity);
  return entity;
}

}  // namespace engine
