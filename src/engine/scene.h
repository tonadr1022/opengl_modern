#pragma once

#include <entt/entity/registry.hpp>

#include "engine/pch.h"

namespace engine {

struct Timestep;
class Engine;
class Event;
struct System;

namespace gfx {
struct RenderViewInfo;
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
  gfx::RenderViewInfo GetViewInfo();
  entt::entity MakeDynamicEntity();
  entt::entity MakeStaticEntity();

  // [[nodiscard]] Entity CreateEntity();
  // [[nodiscard]] Entity CreateEntity(std::string_view tag);
  // [[nodiscard]] Entity GetEntity(std::string_view tag);
  entt::registry registry;
};
}  // namespace engine
