#pragma once

#include <engine/application/input.h>
#include <engine/pch.h>

#include <entt/entity/registry.hpp>

struct Timestep;

class Scene {
 public:
  Scene() = default;
  explicit Scene(std::string name);
  virtual ~Scene() = default;
  [[nodiscard]] const std::string& GetName() const { return name_; }
  virtual void OnUpdate(Timestep timestep);
  virtual void OnKeyEvent(const KeyEvent& e);
  virtual void Load() = 0;
  void Shutdown();

  // [[nodiscard]] Entity CreateEntity();
  // [[nodiscard]] Entity CreateEntity(std::string_view tag);
  // [[nodiscard]] Entity GetEntity(std::string_view tag);
  entt::registry registry_;

 private:
  friend class Entity;
  std::string name_;
};
