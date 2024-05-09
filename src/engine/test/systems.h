#pragma once

#include <entt/entity/fwd.hpp>
#include <glm/fwd.hpp>
#include <vector>

#include "engine/ecs/system/isystem.h"

namespace engine {
struct Timestep;
struct Event;
class WindowSystem;
namespace component {
struct FPSCamera;
struct Transform;
struct Transform;
struct MeshMaterial;
}  // namespace component
}  // namespace engine

namespace ecs {

std::vector<std::string>& GetModelPaths();
void LoadCamera(entt::registry& registry, engine::WindowSystem& window_system);
void ModelViewerLoadModel(entt::registry& registry, engine::component::Transform& transform,
                          const std::vector<engine::component::MeshMaterial>& mesh_materials);

struct CameraSystem : public engine::ecs::ISystem {
  void OnUpdate(engine::Timestep timestep);
  bool OnEvent(const engine::Event& e);
  void OnImGui();
};

};  // namespace ecs
