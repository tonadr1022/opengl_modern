#include "scene_main.h"

#include <engine/application/event.h>
#include <engine/application/key_codes.h>
#include <engine/ecs/component/renderer_components.h>
#include <engine/renderer/material.h>
#include <engine/renderer/resource/material_manager.h>
#include <engine/renderer/resource/mesh_manager.h>
#include <engine/renderer/resource/paths.h>
#include <engine/renderer/resource/shader_manager.h>
#include <imgui.h>

SceneMain::SceneMain() : Scene("main") {}

void SceneMain::OnEvent(Event& e) {
  switch (e.type) {
    case Event::KeyPressed:
      if (e.key.code == KeyCode::B) {
        engine_->LoadScene("test2");
      }
    default:
      break;
  }
}

void SceneMain::OnUpdate(Timestep timestep) {}

void SceneMain::Load() {
  auto tri = registry_.create();
  int a = 5;
  component::Transform t;
  t.SetTranslation({0.1, 0.1, 0});
  registry_.emplace<component::Transform>(tri, t);
  gfx::MeshID mesh_id = gfx::mesh_manager::LoadShape(gfx::ShapeType::Cube);
  registry_.emplace<component::Mesh>(tri, mesh_id);

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
  // auto color_only_mat = gfx::material_manager::AddMaterial(gfx::ColorMaterial{{1.f, 1.f, 1.f}});
  // registry_.remove<component::Material>(tri);
  // registry_.emplace<component::Material>(tri, color_only_mat);
  // auto view = registry_.view<component::Material>();
}
