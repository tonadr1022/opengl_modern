#pragma once

#include <entt/entity/fwd.hpp>
#include <glm/fwd.hpp>

namespace engine {
struct Timestep;
struct Event;
namespace component {
struct FPSCamera;
struct Transform;
}  // namespace component
}  // namespace engine

namespace ecs {

struct CameraSystem {
  void OnUpdate(entt::registry& registry, engine::Timestep timestep);
  bool OnEvent(entt::registry& registry, const engine::Event& e);
  void OnImGui(entt::registry& registry);
};

};  // namespace ecs
