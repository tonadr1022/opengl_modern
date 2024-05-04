#pragma once

#include <entt/entity/registry.hpp>

#include "engine/ecs/system/system.h"
#include "engine/pch.h"

namespace engine {

struct Timestep;
class Engine;
class Event;
struct System;

namespace gfx {
struct ViewInfo;
}

class Scene {
  friend class Engine;

 public:
  Scene();
  virtual ~Scene();
  virtual void OnUpdate(Timestep timestep);
  virtual void OnImGuiRender();
  virtual void OnEvent(const Event& e);
  gfx::ViewInfo GetViewInfo();

  // [[nodiscard]] Entity CreateEntity();
  // [[nodiscard]] Entity CreateEntity(std::string_view tag);
  // [[nodiscard]] Entity GetEntity(std::string_view tag);
  entt::registry registry;
  std::vector<System> systems;

 protected:
  // Engine* engine_{nullptr};
};
}  // namespace engine
