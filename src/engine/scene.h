#pragma once

#include <entt/entity/registry.hpp>

#include "engine/renderer/renderer_types.h"

namespace engine {

struct Timestep;
class Engine;
class Event;
struct System;
class MaterialManager;
class MeshManager;
class WindowSystem;

namespace gfx {
struct RenderViewInfo;
}

namespace ecs {
struct ISystem;
}

class Scene {
  friend class Engine;

 public:
  Scene();
  virtual void Init();
  void InitSystems(std::vector<ecs::ISystem*>& systems);
  virtual ~Scene();
  virtual void OnUpdate(Timestep timestep);
  virtual void OnFixedUpdate(Timestep timestep);
  virtual void OnImGuiRender();
  virtual void OnEvent(const Event& e);
  gfx::RenderViewInfo GetViewInfo();
  entt::entity MakeDynamicEntity();
  entt::entity MakeStaticEntity();

  // [[nodiscard]] Entity CreateEntity();
  // [[nodiscard]] Entity CreateEntity(std::string_view tag);
  // [[nodiscard]] Entity GetEntity(std::string_view tag);

  entt::registry registry;

 protected:
  /** @brief engine will ensure this exists on scene load */
  MaterialManager* material_manager_{nullptr};
  /** @brief engine will ensure this exists on scene load */
  MeshManager* mesh_manager_{nullptr};
  /** @brief engine will ensure this exists on scene load */
  WindowSystem* window_system_{nullptr};
};
}  // namespace engine
