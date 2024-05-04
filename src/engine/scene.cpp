#include "scene.h"

#include "engine/application/engine.h"
#include "engine/renderer/renderer.h"
#include "engine/timestep.h"

namespace engine {

Scene::Scene() {
  auto camera_matrices_entity = registry.create();
  registry.emplace<entt::tag<entt::hashed_string{"view_info"}>>(camera_matrices_entity);
  registry.emplace<gfx::ViewInfo>(camera_matrices_entity);
}

void Scene::OnEvent(const Event& e) {}

void Scene::OnUpdate(Timestep timestep) {}

void Scene::OnImGuiRender() {};

Scene::~Scene() { registry.clear(); }

gfx::ViewInfo Scene::GetViewInfo() {
  auto view = registry.view<entt::tag<entt::hashed_string{"view_info"}>>();
  auto& view_info = registry.get<gfx::ViewInfo>(view.front());
  return view_info;
}

}  // namespace engine
