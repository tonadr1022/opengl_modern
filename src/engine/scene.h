#pragma once

#include <entt/entity/registry.hpp>

#include "engine/core/base.h"
#include "engine/renderer/renderer_types.h"

namespace engine {

class Engine;
class Event;
struct System;
class MaterialManager;
class MeshManager;
class WindowManager;

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
  virtual ~Scene();
  virtual void OnUpdate(Timestep timestep);
  virtual void OnFixedUpdate(Timestep timestep);
  virtual void OnImGuiRender();
  virtual void OnEvent(const Event& e);
  // TODO(tony): make entity class or a factory
  entt::entity MakeDynamicEntity();
  entt::entity MakeStaticEntity();

  entt::registry registry;
  // each scene contains state for how the renderer should render the scene
  // including camera matrices, renderer settings, etc
  gfx::RenderViewInfo render_view_info;

 protected:
  // Helper function so scenes don't need to include engine
  void LoadScene(std::unique_ptr<Scene> scene);
};
}  // namespace engine
