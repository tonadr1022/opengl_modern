#pragma once

#include <entt/entity/registry.hpp>

#include "engine/pch.h"

class Entity;
struct Timestep;

class Scene {
 public:
  Scene() = default;
  explicit Scene(std::string name);
  [[nodiscard]] const std::string& GetName() const { return name_; }
  virtual void OnUpdate(Timestep timestep);
  virtual void OnKeyEvent(int key, int action, int mods);

  // [[nodiscard]] Entity CreateEntity();
  // [[nodiscard]] Entity CreateEntity(std::string_view tag);
  // [[nodiscard]] Entity GetEntity(std::string_view tag);
  entt::registry registry_;

 private:
  friend class Entity;
  std::string name_;
};
