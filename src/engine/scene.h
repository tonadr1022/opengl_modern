#pragma once

#include <engine/application/input.h>
#include <engine/pch.h>

#include <entt/entity/registry.hpp>

#include "engine/renderer/renderer.h"

namespace engine {

struct Timestep;
class Engine;
class Event;

class Scene {
  friend class Engine;

 public:
  Scene() = default;
  explicit Scene(std::string name);
  virtual ~Scene() = default;
  [[nodiscard]] const std::string& GetName() const { return name_; }
  virtual void OnUpdate(Timestep timestep);
  virtual void OnImGuiRender();
  virtual void OnEvent(const Event& e);
  virtual void Load() = 0;
  void Shutdown();

  // [[nodiscard]] Entity CreateEntity();
  // [[nodiscard]] Entity CreateEntity(std::string_view tag);
  // [[nodiscard]] Entity GetEntity(std::string_view tag);
  entt::registry registry;
  gfx::CameraMatrices current_camera_matrices;

 protected:
  Engine* engine_{nullptr};

  std::string name_;
};
}  // namespace engine
