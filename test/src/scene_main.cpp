#include "scene_main.h"

#include <engine/application/event.h>
#include <engine/application/key_codes.h>
#include <engine/ecs/component/camera.h>
#include <engine/ecs/component/core_components.h>
#include <engine/ecs/component/renderer_components.h>
#include <engine/ecs/system/window_system.h>
#include <engine/renderer/material.h>
#include <engine/resource/material_manager.h>
#include <engine/resource/mesh_manager.h>
#include <engine/resource/paths.h>
#include <engine/resource/shader_manager.h>
#include <imgui.h>

#include <entt/core/hashed_string.hpp>
#include <entt/entt.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "components.h"
#include "engine/ecs/component/transform.h"
#include "systems.h"

SceneMain::SceneMain() {
  glm::vec3 iter{0, 0, 0};
  engine::MeshID mesh_id = engine::MeshManager::LoadShape(engine::ShapeType::Cube);
  engine::MaterialData mat;
  mat.diffuse = {1., 0., 1.};
  engine::MaterialID color_only_mat = engine::MaterialManager::AddMaterial(mat);
  component::Transform t;
  float r = 0;

  // engine::MaterialData m;
  // m.diffuse = {1, 1, 0};
  // auto ship = registry.create();
  // auto captain = registry.create();
  // registry.emplace<component::Children>(ship).children = {captain};
  // registry.emplace<component::Parent>(captain).parent = ship;
  //
  // registry.emplace<component::Transform>(ship, t);
  // registry.emplace<component::LocalTransform>(ship);
  // registry.emplace<component::ModelMatrix>(ship);
  // registry.emplace<component::Mesh>(ship, mesh_id);
  // registry.emplace<component::Material>(ship, m);
  //
  // registry.emplace<component::Transform>(captain, t);
  // registry.emplace<component::LocalTransform>(captain, t);
  // registry.emplace<component::ModelMatrix>(captain);
  // registry.emplace<component::Mesh>(captain, mesh_id);
  // m.diffuse = {0, 1, 1};
  // registry.emplace<component::Material>(captain, m);

  std::vector<component::Material> data;
  int num_mats = 100;
  data.reserve(num_mats);
  engine::MaterialData d;
  for (int i = 0; i < num_mats; i++) {
    d.diffuse = {rand() % num_mats / static_cast<float>(num_mats), rand() % num_mats / num_mats,
                 rand() % num_mats / num_mats};
    engine::MaterialID itermat = engine::MaterialManager::AddMaterial(d);
    data.push_back({itermat});
  }

  for (iter.x = -40; iter.x <= 40; iter.x++) {
    for (iter.y = -40; iter.y <= 40; iter.y++) {
      for (iter.z = -4; iter.z <= 10; iter.z++) {
        entt::entity tri{};
        if (static_cast<int>(iter.z + iter.x) % 2 == 0) {
          tri = MakeDynamicEntity();
        } else {
          tri = MakeStaticEntity();
        }
        t.SetTranslation({iter.x * 2, iter.y * 2, iter.z * 2});
        registry.emplace<component::Transform>(tri, t);
        registry.emplace<component::ModelMatrix>(tri);
        registry.emplace<component::Mesh>(tri, mesh_id);
        registry.emplace<component::Material>(tri, data[rand() % num_mats]);
      }
    }
  }

  bool start_fps_focus = true;
  auto player = registry.create();
  registry.emplace<Player>(player).fps_focused = start_fps_focus;
  WindowSystem::Get().SetCursorVisible(!start_fps_focus);

  component::FPSCamera fps_cam;
  fps_cam.position = {2, 1, 1};
  registry.emplace<component::FPSCamera>(player, fps_cam);
}

void SceneMain::OnEvent(const engine::Event& e) { ecs::fps_cam_sys::OnEvent(registry, e); }

void SceneMain::OnImGuiRender() {
  ImGui::Begin("Scene");
  auto player_entity = registry.view<Player>().front();
  if (registry.any_of<component::FPSCamera>(player_entity)) {
    auto& camera = registry.get<component::FPSCamera>(player_entity);
    ecs::fps_cam_sys::FPSCamImGui(camera);
  }

  // auto materials = registry.group<component::Material>();
  // materials.each([](component::Material& material) {
  //   auto& mat = gfx::MaterialManager::GetMaterial(material.handle);
  //   ImGui::SliderFloat3(std::string("Diffuse###" + std::to_string(material.handle)).c_str(),
  //                       glm::value_ptr(mat.diffuse), 0.0f, 1.0f);
  // });

  ImGui::End();
}

void SceneMain::OnUpdate(engine::Timestep timestep) { ecs::fps_cam_sys::Run(registry, timestep); }
