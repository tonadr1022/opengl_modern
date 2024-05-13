#include "scene_main.h"

#include <imgui.h>
#include <imgui_file/ImGuiFileDialog.h>

#include "engine/application/event.h"
#include "engine/core/base.h"
#include "engine/ecs/component/transform.h"
#include "engine/resource/resource.h"
#include "engine/resource/resource_manager.h"
#include "engine/test/scene_2.h"
#include "engine/window_manager.h"
#include "systems.h"

SceneMain::SceneMain() : camera_system(registry, render_view_info) {}

void SceneMain::Init() {
  camera_system.enabled = false;
  camera_system.camera_mode = engine::CameraMode::FPS;
  player_entity_ = registry.create();
  camera_system.InitDefaultCamera(player_entity_);

  std::string model_string =
      "/home/tony/dep/models/glTF-Sample-Assets/Models/Sponza/glTF/Sponza.gltf";
  // "/home/tony/dep/models/glTF-Sample-Assets/Models/WaterBottle/glTF/WaterBottle.gltf";

  AssetHandle model_handle = engine::ModelManager::Get().LoadModel(model_string);
  auto& model = engine::ModelManager::Get().GetModel(model_handle);

  glm::vec3 iter{0};
  auto scale = glm::vec3(.01);
  int c = 1;
  for (iter.z = -c; iter.z <= c; iter.z++) {
    for (iter.x = -c; iter.x <= c; iter.x++) {
      for (auto m : model.meshes) {
        engine::component::Transform t;
        t.SetScale(scale);
        auto ent = registry.create();
        t.SetTranslation({iter.x * 50, iter.y, iter.z * 50});
        registry.emplace<engine::Mesh>(ent, m);
        registry.emplace<engine::component::Transform>(ent, t);
        registry.emplace<engine::component::ModelMatrix>(ent);
      }
    }
  }
}

using engine::EventType;
using engine::KeyCode;
void SceneMain::OnEvent(const engine::Event& e) {
  switch (e.type) {
    case EventType::KeyPressed:
      if (e.key.code == KeyCode::M) {
        camera_system.enabled = !camera_system.enabled;
        if (!camera_system.enabled) engine::WindowManager::Get().CenterCursor();
        engine::WindowManager::Get().SetCursorVisibility(!camera_system.enabled);
        break;
      } else if (e.key.code == KeyCode::B) {
        LoadScene(std::make_unique<Scene2>());
        break;
      }
      break;
    default:
      break;
  }
}

void SceneMain::OnUpdate(Timestep timestep) {
  if (camera_system.enabled) camera_system.OnUpdate(timestep);
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
  if (ImGui::TreeNode("Systems")) {
    ImGui::Checkbox("Camera", &camera_system.enabled);
  }

  camera_system.OnImGui();

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
    auto model_handle = engine::ModelManager::Get().LoadModel(strings[i]);
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
      auto model_handle = engine::ModelManager::Get().LoadModel(file_path_name);
      auto& model = engine::ModelManager::Get().GetModel(model_handle);
      ModelViewerLoadModel(registry, t, model);
    }
    ImGuiFileDialog::Instance()->Close();
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
//     d.base_color = {rand() % num_mats / static_cast<float>(num_mats), rand() % num_mats /
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