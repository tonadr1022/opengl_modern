#include "scene_2.h"

#include "engine/application/event.h"
#include "engine/ecs/component/transform.h"
#include "engine/resource/model_manager.h"
#include "engine/window_manager.h"
#include "scene_main.h"

using engine::KeyCode;
void Scene2::OnEvent(const engine::Event& e) {
  switch (e.type) {
    case engine::EventType::kKeyPressed:
      if (e.key.code == engine::KeyCode::B) {
        LoadScene(std::make_unique<SceneMain>());
      } else if (e.key.code == KeyCode::M) {
        camera_system.enabled = !camera_system.enabled;
        if (!camera_system.enabled) engine::WindowManager::Get().CenterCursor();
        engine::WindowManager::Get().SetCursorVisibility(!camera_system.enabled);
        break;
      } else if (e.key.code == KeyCode::B) {
        default:
          break;
      }
  }
}

Scene2::Scene2() : camera_system(registry, render_view_info) {}

void Scene2::OnUpdate(Timestep timestep) {
  if (camera_system.enabled) camera_system.OnUpdate(timestep);
}

void Scene2::Init() {
  camera_system.enabled = false;
  camera_system.camera_mode = engine::CameraMode::FPS;
  camera_system.InitDefaultCamera();

  std::string model_string =
      // "/home/tony/dep/models/glTF-Sample-Assets/Models/WaterBottle/glTF/WaterBottle.gltf";
      "/home/tony/dep/models/glTF-Sample-Assets/Models/Cube/glTF/Cube.gltf";

  AssetHandle model_handle = engine::ModelManager::Get().LoadModel({model_string});
  auto& model = engine::ModelManager::Get().GetModel(model_handle);

  glm::vec3 iter{0};
  auto scale = glm::vec3(1);
  int c = 10;
  for (iter.z = -c; iter.z <= c; iter.z++) {
    for (iter.x = -c; iter.x <= c; iter.x++) {
      engine::component::Transform t;
      t.SetScale(scale);
      auto ent = registry.create();
      t.SetTranslation({iter.x * 10, iter.y, iter.z * 10});
      registry.emplace<engine::Mesh>(ent, model.meshes[0]);
      registry.emplace<engine::component::Transform>(ent, t);
      registry.emplace<engine::component::ModelMatrix>(ent);
    }
  }

  // plane
  engine::component::Transform t;
  t.SetScale({100, 0.1, 100});
  t.SetTranslation({0, -5, 0});
  auto ent = registry.create();
  registry.emplace<engine::Mesh>(ent, model.meshes[0]);
  registry.emplace<engine::component::Transform>(ent, t);
  registry.emplace<engine::component::ModelMatrix>(ent);
}
