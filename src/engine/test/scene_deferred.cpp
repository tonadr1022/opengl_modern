#include "scene_deferred.h"

#include "engine/application/event.h"
#include "engine/ecs/component/transform.h"
#include "engine/resource/model_manager.h"
#include "engine/resource/paths.h"
#include "engine/window_manager.h"

SceneDeferred::SceneDeferred() : camera_system(registry, render_view_info) {}

void SceneDeferred::Init() {
  camera_system.camera_mode = engine::CameraMode::FPS;
  camera_system.InitDefaultCamera({0, 5, 0}, {-1, 0, 0});

  std::string model_string = GET_MODEL_PATH("backpack/backpack.obj");
  AssetHandle model_handle = engine::ModelManager::Get().LoadModel(model_string);
  auto& model = engine::ModelManager::Get().GetModel(model_handle);

  int c = 1;
  glm::vec3 iter{0};
  for (iter.z = -c; iter.z <= c; iter.z++) {
    for (iter.x = -c; iter.x <= c; iter.x++) {
      for (auto m : model.meshes) {
        engine::component::Transform t;
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
void SceneDeferred::OnEvent(const engine::Event& e) {
  switch (e.type) {
    case EventType::kKeyPressed:
      if (e.key.code == KeyCode::M) {
        camera_system.on_ = !camera_system.on_;
        if (!camera_system.on_) engine::WindowManager::Get().CenterCursor();
        engine::WindowManager::Get().SetCursorVisibility(!camera_system.on_);
        break;
      }
    default:
      break;
  }
}
void SceneDeferred::OnUpdate(Timestep timestep) { camera_system.OnUpdate(timestep); }
