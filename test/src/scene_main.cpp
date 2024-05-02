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

#include <glm/gtc/type_ptr.hpp>

#include "systems.h"

SceneMain::SceneMain() : Scene("main") {}

using namespace engine;

void SceneMain::OnEvent(const Event& e) {
  auto player_entity = registry.view<Player>().front();
  if (!registry.any_of<component::FPSCamera>(player_entity)) {
    std::cout << "return\n";
    return;
  }
  auto& camera = registry.get<component::FPSCamera>(player_entity);
  switch (e.type) {
    case Event::Type::KeyPressed:
      if (e.key.code == KeyCode::M) {
        fps_focused_ = !fps_focused_;
        if (!fps_focused_) engine_->window_system_->CenterCursor();
        engine_->window_system_->SetCursorVisible(!fps_focused_);
      } else if (e.key.code == KeyCode::B) {
        engine_->LoadScene("test2");
      }
      break;
    case Event::Type::MouseScrolled:
      if (fps_focused_) ecs::fps_cam_sys::OnScroll(camera, e.scroll.offset);
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

  ImGui::BeginChild("Materials");

  ImGui::EndChild();
  // auto materials = registry.group<component::Material>();
  // materials.each([](component::Material& material) {
  //   auto& mat = gfx::MaterialManager::GetMaterial(material.handle);
  //   ImGui::SliderFloat3(std::string("Diffuse###" + std::to_string(material.handle)).c_str(),
  //                       glm::value_ptr(mat.diffuse), 0.0f, 1.0f);
  // });
  ImGui::End();
}

void SceneMain::OnUpdate(Timestep timestep) {
  auto player_entity = registry.view<Player>().front();
  if (!registry.any_of<component::FPSCamera>(player_entity)) return;
  auto& camera = registry.get<component::FPSCamera>(player_entity);
  if (fps_focused_) ecs::fps_cam_sys::OnUpdate(camera, timestep);
  view_matrix_ = glm::lookAt(camera.position, camera.position + camera.front, {0., 1., 0.});
  auto dims = engine_->window_system_->GetWindowDimensions();
  float aspect_ratio = static_cast<float>(dims.x) / static_cast<float>(dims.y);
  projection_matrix_ =
      glm::perspective(glm::radians(camera.fov), aspect_ratio, camera.near_plane, camera.far_plane);
}

void SceneMain::Load() {
  engine_->window_system_->SetCursorVisible(!fps_focused_);
  glm::vec3 iter{0, 0, 0};
  gfx::MeshID mesh_id = gfx::MeshManager::LoadShape(gfx::ShapeType::Cube);
  gfx::MaterialData mat;
  mat.diffuse = {1., 0., 1.};
  gfx::MaterialID color_only_mat = gfx::MaterialManager::AddMaterial(mat);
  for (iter.x = -50; iter.x <= 50; iter.x++) {
    auto tri = registry.create();
    component::Transform t;
    t.SetTranslation({iter.x * 2, 0, 0});
    registry.emplace<component::Transform>(tri, t);
    registry.emplace<component::ModelMatrix>(tri);
    registry.emplace<component::Mesh>(tri, mesh_id);
    registry.emplace<component::Material>(tri, color_only_mat);
  }
  auto player = registry.create();
  registry.emplace<Player>(player);
  component::FPSCamera fps_cam;
  fps_cam.position = {2, 1, 1};
  registry.emplace<component::FPSCamera>(player, fps_cam);
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
  // registry_.remove<component::Material>(tri);
}
