#include "scene_main.h"

#include <engine/application/event.h>
#include <engine/application/key_codes.h>
#include <engine/ecs/component/camera.h>
#include <engine/ecs/component/core_components.h>
#include <engine/ecs/component/renderer_components.h>
#include <engine/ecs/system/window_system.h>
#include <engine/renderer/material.h>
#include <engine/renderer/resource/material_manager.h>
#include <engine/renderer/resource/mesh_manager.h>
#include <engine/renderer/resource/paths.h>
#include <engine/renderer/resource/shader_manager.h>
#include <imgui.h>

#include "systems.h"

SceneMain::SceneMain() : Scene("main") {}

void SceneMain::OnEvent(const Event& e) {
  auto player_entity = registry.view<Player>().front();
  if (!registry.any_of<component::FPSCamera>(player_entity)) {
    std::cout << "return\n";
    return;
  }
  auto& camera = registry.get<component::FPSCamera>(player_entity);
  switch (e.type) {
    case Event::Type::KeyPressed:
      if (e.key.code == KeyCode::B) {
        engine_->LoadScene("test2");
      } else if (e.key.code == KeyCode::Escape) {
        bool visible = engine_->window_system_->GetCursorVisible();
        std::cout << visible << "  visible\n";
        engine_->window_system_->SetCursorVisible(!visible);
      }
    case Event::Type::MouseMoved:
      ecs::fps_cam_sys::OnMouseMoved(camera, {e.mouse_pos.x, e.mouse_pos.y});
      break;
    case Event::Type::MouseScrolled:
      ecs::fps_cam_sys::OnScroll(camera, e.scroll.offset);
      break;
    default:
      break;
  }
}
void SceneMain::OnImGuiRender() {
  ImGui::Begin("Scene");
  auto player_entity = registry.view<Player>().front();
  if (registry.any_of<component::FPSCamera>(player_entity)) {
    auto& camera = registry.get<component::FPSCamera>(player_entity);
    ecs::fps_cam_sys::OnImGui(camera);
  }
  ImGui::End();
}

void SceneMain::OnUpdate(Timestep timestep) {
  auto player_entity = registry.view<Player>().front();
  if (registry.any_of<component::FPSCamera>(player_entity)) {
    auto& camera = registry.get<component::FPSCamera>(player_entity);
    ecs::fps_cam_sys::OnUpdate(camera, timestep);
    if (!engine_->window_system_->GetCursorVisible()) {
      engine_->window_system_->CenterCursor();
    }
    view_matrix_ = ecs::fps_cam_sys::GetView(camera);
    projection_matrix_ = ecs::fps_cam_sys::GetProjection(camera.proj_mat_settings);
  }
}

void SceneMain::Load() {
  auto tri = registry.create();
  int a = 5;
  component::Transform t;
  t.SetTranslation({0.1, 0.1, 0});
  registry.emplace<component::Transform>(tri, t);
  registry.emplace<component::ModelMatrix>(tri);
  gfx::MeshID mesh_id = gfx::mesh_manager::LoadShape(gfx::ShapeType::Cube);
  registry.emplace<component::Mesh>(tri, mesh_id);
  auto player = registry.create();
  registry.emplace<Player>(player);
  registry.emplace<component::FPSCamera>(player);
  // gfx::MaterialCreateInfo color_mat_create_info;
  //
  // std::vector<gfx::ShaderCreateInfo> color_shader_create_info{
  //     {GET_SHADER_PATH("color.vs.glsl"), gfx::ShaderType::Vertex},
  //     {GET_SHADER_PATH("color.fs.glsl"), gfx::ShaderType::Fragment},
  // };
  //
  // // shader with uniform setter function
  // gfx::ShaderManager::AddShader("color", color_shader_create_info);
  // color_mat_create_info.shader_id = "color";
  // auto color_mat = gfx::material_manager::AddMaterial(color_mat_create_info);
  // registry_.emplace<component::Material>(tri, color_mat);
  //
  // // Color only, no shader
  auto color_only_mat = gfx::material_manager::AddMaterial(gfx::ColorMaterial{{1.f, 1.f, 1.f}});
  // registry_.remove<component::Material>(tri);
  registry.emplace<component::Material>(tri, color_only_mat);
}
