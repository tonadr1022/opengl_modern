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
        camera_system.on_ = !camera_system.on_;
        if (!camera_system.on_) engine::WindowManager::Get().CenterCursor();
        engine::WindowManager::Get().SetCursorVisibility(!camera_system.on_);
        break;
      } else if (e.key.code == KeyCode::B) {
        default:
          break;
      }
  }
}

Scene2::Scene2() : camera_system(registry, render_view_info) {}

void Scene2::OnUpdate(Timestep timestep) {
  Scene::OnUpdate(timestep);
  if (camera_system.enabled) camera_system.OnUpdate(timestep);
}

void Scene2::Init() {
  camera_system.camera_mode = engine::CameraMode::FPS;
  camera_system.InitDefaultCamera();

  std::string model_string =
      // "/home/tony/dep/models/glTF-Sample-Assets/Models/WaterBottle/glTF/WaterBottle.gltf";
      "/home/tony/dep/models/glTF-Sample-Assets/Models/Cube/glTF/Cube.gltf";

  AssetHandle model_handle = engine::ModelManager::Get().LoadModel(model_string);
  auto& model = engine::ModelManager::Get().GetModel(model_handle);

  auto scale = glm::vec3(.5);
  std::vector<glm::vec3> positions = {{0, 2.5, 0}, {2, 0.6, 1}, {-1, 0.6, 2}};
  for (auto& pos : positions) {
    engine::component::Transform t;
    t.SetScale(scale);
    auto ent = registry.create();
    t.SetTranslation(pos);
    registry.emplace<engine::Mesh>(ent, model.meshes[0]);
    registry.emplace<engine::component::Transform>(ent, t);
    registry.emplace<engine::component::ModelMatrix>(ent);
  }

  // plane
  engine::component::Transform t;
  t.SetScale({25, 0.1, 25});
  t.SetTranslation({0, 0, 0});
  auto ent = registry.create();
  registry.emplace<engine::Mesh>(ent, model.meshes[0]);
  registry.emplace<engine::component::Transform>(ent, t);
  registry.emplace<engine::component::ModelMatrix>(ent);
}
