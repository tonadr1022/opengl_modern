#include "scene.h"

#include <imgui.h>

#include "engine/application/engine.h"
#include "engine/ecs/component/transform.h"
#include "engine/renderer/light.h"
#include "engine/renderer/renderer.h"

namespace engine {

Scene::Scene() = default;

Scene::~Scene() { registry.clear(); }

void Scene::LoadScene(std::unique_ptr<Scene> scene) { Engine::Get().LoadScene(std::move(scene)); }

void Scene::OnEvent(const Event& e) {}

void Scene::OnUpdate(Timestep timestep) {}

void Scene::OnFixedUpdate(Timestep timestep) {}

void Scene::OnImGuiRender() {
  // ImGui::ShowDemoWindow();
  if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen)) {
    auto lights = registry.view<PointLight>();
    int i = 0;
    lights.each([&](PointLight& light) {
      std::string id = "point_light_" + std::to_string(i++);
      ImGui::PushID(id.c_str());
      ImGui::DragFloat3("Position##", &light.position.x, 0.1);
      ImGui::DragFloat3("Color", &light.color.x, 0.01, 0, 1);
      ImGui::DragFloat("Intensity", &light.intensity, 1, 0, 1000);
      ImGui::PopID();
    });
  };
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
