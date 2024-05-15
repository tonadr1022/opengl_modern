#include "scene_main.h"

#include <imgui.h>
#include <imgui_file/ImGuiFileDialog.h>

#include "engine/application/event.h"
#include "engine/core/base.h"
#include "engine/ecs/component/transform.h"
#include "engine/graphics_manager.h"
#include "engine/renderer/light.h"
#include "engine/renderer/renderer.h"
#include "engine/resource/material_manager.h"
#include "engine/resource/model_loader.h"
#include "engine/resource/model_manager.h"
#include "engine/resource/paths.h"
#include "engine/resource/resource.h"
#include "engine/test/scene_2.h"
#include "engine/window_manager.h"
#include "systems.h"

void SceneMain::LoadSponza() {
  camera_system.InitDefaultCamera({0, 5, 0}, {-1, 0, 0});
  std::string model_string =
      "/home/tony/dep/models/glTF-Sample-Assets/Models/Sponza/glTF/Sponza.gltf";
  AssetHandle model_handle = engine::ModelManager::Get().LoadModel({model_string});
  auto& model = engine::ModelManager::Get().GetModel(model_handle);
  auto scale = glm::vec3(.1);
  for (auto m : model.meshes) {
    engine::component::Transform t;
    t.SetScale(scale);
    auto ent = registry.create();
    registry.emplace<engine::Mesh>(ent, m);
    registry.emplace<engine::component::Transform>(ent, t);
    registry.emplace<engine::component::ModelMatrix>(ent);
  }
  glm::vec3 light_positions[] = {
      glm::vec3(0, 1.0f, 0), glm::vec3(1, 1.0f, 0), glm::vec3(-1, 1.0f, 0), glm::vec3(0, 1.0f, 1),
      // glm::vec3(0, 2, -1)
  };
  engine::PointLight light;
  light.color = {1, 1, 1};
  for (auto& pos : light_positions) {
    auto ent = registry.create();
    light.position = glm::vec4{pos.x, pos.y, pos.z, 0};
    registry.emplace<engine::PointLight>(ent, light);
  }
}

void SceneMain::LoadSpheres() {
  camera_system.InitDefaultCamera();
  std::string model_string = GET_MODEL_PATH("ball.obj");
  AssetHandle model_handle = engine::ModelManager::Get().LoadModel({model_string});
  auto& model = engine::ModelManager::Get().GetModel(model_handle);
  auto scale = glm::vec3(.01);
  // engine::Mesh m = engine::ShapeLoader::LoadSphere();
  engine::Mesh m = model.meshes[0];
  engine::component::Transform t;
  int num_cols = 7;
  int num_rows = 7;
  float spacing{2.5};
  for (int row = 0; row < num_rows; row++) {
    for (int col = 0; col < num_cols; col++) {
      engine::MaterialCreateInfo mat;
      mat.base_color = {1, 1, 1, 1};
      mat.roughness = glm::clamp(static_cast<float>(col) / static_cast<float>(num_cols), .05f, 1.f);
      mat.metallic = static_cast<float>(row) / static_cast<float>(num_rows);
      auto mat_handle = engine::MaterialManager::Get().AddMaterial(mat);
      m.material_handle = mat_handle;
      t.SetTranslation({(col - num_cols / 2) * spacing, (row - num_rows / 2) * spacing, 0.0f});
      t.SetScale(scale);
      auto ent = registry.create();
      registry.emplace<engine::Mesh>(ent, m);
      registry.emplace<engine::component::Transform>(ent, t);
      registry.emplace<engine::component::ModelMatrix>(ent);
    }
  }
  glm::vec3 light_positions[] = {
      glm::vec3(-10.0f, 10.0f, 0.0f),
      glm::vec3(10.0f, 10.0f, 10.0f),
      glm::vec3(-10.0f, -10.0f, 10.0f),
      glm::vec3(10.0f, -10.0f, 10.0f),
  };

  engine::PointLight light;
  light.color = {1, 1, 1};
  for (auto& pos : light_positions) {
    auto ent = registry.create();
    light.position = glm::vec4{pos.x, pos.y, pos.z, 0};
    registry.emplace<engine::PointLight>(ent, light);
    engine::MaterialCreateInfo mat;
    mat.base_color = glm::vec4{light.color.x, light.color.y, light.color.z, 1};
    registry.emplace<engine::Mesh>(ent, m);
    engine::component::Transform t;
    t.SetScale({0.005, 0.005, 0.005});
    t.SetTranslation(light.position);
    registry.emplace<engine::component::Transform>(ent, t);
    registry.emplace<engine::component::ModelMatrix>(ent);
  }
  engine::DirectionalLight dir_light;
  dir_light.color = {1, 0, 0};
}

void SceneMain::LoadSpheres2() {
  camera_system.InitDefaultCamera();
  std::string model_string =
      "/home/tony/personal/cpp/opengl_modern/resources/models/spheres_demo/scene.gltf";
  AssetHandle model_handle = engine::ModelManager::Get().LoadModel(model_string);
  spdlog::info("loading gltf fast");
  engine::ModelLoader::LoadModel2(
      "/home/tony/dep/models/glTF-Sample-Assets/Models/Sponza/glTF/Sponza.gltf");
  spdlog::info("done loading fast");
  auto& model = engine::ModelManager::Get().GetModel(model_handle);
  // auto scale = glm::vec3(.01);
  engine::Mesh m = model.meshes[0];
  engine::component::Transform t;
  // t.SetScale(scale);
  for (auto m : model.meshes) {
    engine::component::Transform t;
    auto ent = registry.create();
    registry.emplace<engine::Mesh>(ent, m);
    registry.emplace<engine::component::Transform>(ent, t);
    registry.emplace<engine::component::ModelMatrix>(ent);
  }
}
// glm::vec3 light_positions[] = {
//     glm::vec3(-10.0f, 10.0f, 0.0f),
//     glm::vec3(10.0f, 10.0f, 10.0f),
//     glm::vec3(-10.0f, -10.0f, 10.0f),
//     glm::vec3(10.0f, -10.0f, 10.0f),
// };
//
// engine::PointLight light;
// light.color = {1, 1, 1};
// for (auto& pos : light_positions) {
//   auto ent = registry.create();
//   light.position = glm::vec4{pos.x, pos.y, pos.z, 0};
//   registry.emplace<engine::PointLight>(ent, light);
//   engine::MaterialCreateInfo mat;
//   mat.base_color = light.color;
//   registry.emplace<engine::Mesh>(ent, m);
//   engine::component::Transform t;
//   t.SetScale({0.005, 0.005, 0.005});
//   t.SetTranslation(light.position);
//   registry.emplace<engine::component::Transform>(ent, t);
//   registry.emplace<engine::component::ModelMatrix>(ent);
// }
// engine::DirectionalLight dir_light;
// dir_light.color = {1, 0, 0};

SceneMain::SceneMain() : camera_system(registry, render_view_info) {}

void SceneMain::Init() {
  camera_system.camera_mode = engine::CameraMode::FPS;
  // camera_system.InitDefaultCamera(player_entity_);
  LoadSponza();
  // LoadSpheres();

  // for (iter.z = -c; iter.z <= c; iter.z++) {
  //   for (iter.x = -c; iter.x <= c; iter.x++) {
  //     for (auto m : model.meshes) {
  //       engine::component::Transform t;
  //       t.SetScale(scale);
  //       auto ent = registry.create();
  //       t.SetTranslation({iter.x * 50, iter.y, iter.z * 50});
  //       registry.emplace<engine::Mesh>(ent, m);
  //       registry.emplace<engine::component::Transform>(ent, t);
  //       registry.emplace<engine::component::ModelMatrix>(ent);
  //     }
  //   }
  // }
}

using engine::EventType;
using engine::KeyCode;
void SceneMain::OnEvent(const engine::Event& e) {
  switch (e.type) {
    case EventType::kKeyPressed:
      if (e.key.code == KeyCode::M) {
        camera_system.on_ = !camera_system.on_;
        if (!camera_system.on_) engine::WindowManager::Get().CenterCursor();
        engine::WindowManager::Get().SetCursorVisibility(!camera_system.on_);
        break;
      } else if (e.key.code == KeyCode::B) {
        LoadScene(std::make_unique<Scene2>());
        break;
      } else if (e.key.code == KeyCode::N) {
        curr_scene_ =
            static_cast<SceneMain::CurrScene>((curr_scene_ + 1) % SceneMain::CurrScene::kCount);
        registry.clear();
        engine::gfx::Renderer::Get().Reset();
        switch (curr_scene_) {
          case kSpheres:
            LoadSpheres();
            break;
          case kSponza:
            LoadSponza();
            break;
          case kSpheres2:
            LoadSpheres2();
            break;
          default:
            break;
        }
      }
      break;
    default:
      break;
  }
}

void SceneMain::OnUpdate(Timestep timestep) {
  camera_system.OnUpdate(timestep);
  auto v = registry.view<engine::PointLight, engine::component::Transform>();
  v.each([&](engine::PointLight& light, engine::component::Transform& transform) {
    transform.SetTranslation(light.position);
  });

  // static float t;
  // t += timestep;
  // auto v = registry.view<PointLight>();
  // v.each(
  //     [&](PointLight& light) { light.position += glm::vec4(sin(t * 5.0) * 5.0, 0.0, 0.0, 0.0);
  //     });
}

void DrawImGuiDropdown(const char* label, std::vector<std::string>& items, int& currentItemIndex) {
  if (ImGui::BeginCombo(label, items[currentItemIndex].data())) {
    for (int i = 0; i < items.size(); i++) {
      bool is_selected = (currentItemIndex == i);
      if (ImGui::Selectable(items[i].data(), is_selected)) currentItemIndex = i;

      // Set the initial focus when opening the combo
      if (is_selected) ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
}

// TODO(tony): clean up and separate
void SceneMain::OnImGuiRender() {
  ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoNavFocus);
  Scene::OnImGuiRender();
  if (ImGui::CollapsingHeader("Systems", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Checkbox("Camera", &camera_system.enabled);
  }

  camera_system.OnImGui();

  if (ImGui::CollapsingHeader("Model Load", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::Button("Open File Dialog")) {
      IGFD::FileDialogConfig config;
      config.path = ".";
      ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".gltf,.obj,.fbx",
                                              config);
    }
    auto& strings = GetModelPaths();
    static int i = 0;
    DrawImGuiDropdown("dropdown", strings, i);
    if (ImGui::Button("Load")) {
      auto v = registry.view<engine::component::Transform>();
      registry.destroy(v.begin(), v.end());
      engine::component::Transform t;
      auto model_handle = engine::ModelManager::Get().LoadModel({strings[i]});
      auto& model = engine::ModelManager::Get().GetModel(model_handle);

      ModelViewerLoadModel(registry, t, model);
    }
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
      if (ImGuiFileDialog::Instance()->IsOk()) {  // action if OK
        std::string file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();
        std::string file_path = ImGuiFileDialog::Instance()->GetCurrentPath();
        auto v = registry.view<engine::component::Transform>();
        registry.destroy(v.begin(), v.end());
        engine::component::Transform t;
        auto model_handle = engine::ModelManager::Get().LoadModel({file_path_name});
        auto& model = engine::ModelManager::Get().GetModel(model_handle);
        ModelViewerLoadModel(registry, t, model);
      }
      ImGuiFileDialog::Instance()->Close();
    }
  }

  ImGui::End();
}

// SceneMain::SceneMain() {
//   glm::vec3 iter{0, 0, 0};
//   engine::MeshID mesh_id = mesh_manager_->LoadShape(engine::ShapeType::Cube);
//   MaterialCreateInfo mat_create_info;
//   mat_create_info.base_color = {1., 0., 1.};
//   engine::MaterialID color_only_mat = material_manager_->AddMaterial(mat_create_info);
//   component::Transform t;
//   float r = 0;
//
//   std::vector<component::Material> data;
//   int num_mats = 100;
//   data.reserve(num_mats);
//   MaterialCreateInfo d;
//   for (int i = 0; i < num_mats; i++) {
//     d.base_color = {rand() %/*  num_mats / static_cast<float>(num_mats), rand() % */ num_mats /
//     num_mats,
//                     rand() % num_mats / num_mats};
//     engine::MaterialID itermat = material_manager_->AddMaterial(d);
//     data.push_back({itermat});
//   }
//
//   for (iter.x = -40; iter.x <= 40; iter.x++) {
//     for (iter.y = -40; iter.y <= 40; iter.y++) {
//       for (iter.z = 0; iter.z <= 0; iter.z++) {
//         entt::entity tri{};
//         if (static_cast<int>(iter.z + iter.x) % 2 == 0) {
//           tri = MakeDynamicEntity();
//         } else {
//           tri = MakeStaticEntity();
//         }
//         t.SetTranslation({iter.x * 2, iter.y * 2, iter.z * 2});
//         registry.emplace<component::Transform>(tri, t);
//         registry.emplace<component::ModelMatrix>(tri);
//         registry.emplace<component::Mesh>(tri, mesh_id);
//         registry.emplace<component::Material>(tri, data[rand() % num_mats]);
//       }
//     }
//   }
//
//   bool start_fps_focus = true;
//   auto player = registry.create();
//   registry.emplace<Player>(player).fps_focused = start_fps_focus;
//   WindowSystem::Get().SetCursorVisible(!start_fps_focus);
//
//   component::FPSCamera fps_cam;
//   fps_cam.position = {2, 1, 1};
//   registry.emplace<component::FPSCamera>(player, fps_cam);
// }
