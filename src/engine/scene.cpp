#include "scene.h"

#include <imgui.h>

#include <glm/fwd.hpp>

#include "engine/application/engine.h"
#include "engine/ecs/component/transform.h"
#include "engine/renderer/light.h"
#include "engine/renderer/renderer.h"

namespace engine {

Scene::Scene() = default;

Scene::~Scene() { registry.clear(); }

void Scene::LoadScene(std::unique_ptr<Scene> scene) { Engine::Get().LoadScene(std::move(scene)); }

void Scene::OnEvent(const Event& e) {}
namespace {
float test{1};
}

void Scene::OnUpdate(Timestep timestep) {
  if (dir_light_settings.circular_rotate) {
    static double t = 0;
    t += timestep * dir_light_settings.rotate_speed;
    dir_light.direction.x = glm::sin(t) * test;
    dir_light.direction.y = -1;
    dir_light.direction.z = glm::cos(t) * test;
  }
}

void Scene::OnFixedUpdate(Timestep timestep) {}

void Scene::OnImGuiRender() {
  // ImGui::ShowDemoWindow();
  if (ImGui::CollapsingHeader("Point Lights", ImGuiTreeNodeFlags_DefaultOpen)) {
    int i = 0;
    ImGui::Checkbox("enabled##point_light_enabled", &render_view_info.point_light_shadows_on);
    for (auto& light : point_lights) {
      std::string id = "point_light_" + std::to_string(i++);
      ImGui::PushID(id.c_str());
      if (ImGui::DragFloat3("Position##", &light.position.x, 0.1)) point_lights_dirty = true;
      if (ImGui::DragFloat3("Color", &light.color.x, 0.01, 0, 1)) point_lights_dirty = true;
      if (ImGui::DragFloat("Intensity", &light.intensity, 1, 0, 1000)) point_lights_dirty = true;
      ImGui::PopID();
    }
  };
  if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Checkbox("enabled##dir_light_enabled", &render_view_info.dir_light_shadows_on);
    ImGui::DragFloat3("direction", &dir_light.direction.x, 0.01, -1, 1);
    ImGui::DragFloat3("color", &dir_light.color.x, 0.01, 0, 1);
    ImGui::Checkbox("rotate circular", &dir_light_settings.circular_rotate);
    ImGui::DragFloat("rotate speed", &dir_light_settings.rotate_speed, 0.01, 0, 2);
    ImGui::DragFloat("angle", &test, 0.01, 0.01, 3.14);
  }
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
