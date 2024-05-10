#include "scene.h"

#include "engine/application/engine.h"
#include "engine/ecs/component/transform.h"

namespace engine {

Scene::Scene() = default;
Scene::~Scene() { registry.clear(); }

void Scene::LoadScene(std::unique_ptr<Scene> scene) { Engine::Get().LoadScene(std::move(scene)); }

void Scene::OnEvent(const Event& e) {}

void Scene::OnUpdate(Timestep timestep) {}

void Scene::OnFixedUpdate(Timestep timestep) {}

void Scene::OnImGuiRender() {};

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
