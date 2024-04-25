#pragma once

#include <entt/entity/registry.hpp>

class Entity;

class Scene {
 public:
  Scene() = default;
  explicit Scene(std::string name);
  [[nodiscard]] const std::string& GetName() const { return name_; }
  Entity CreateEntity(std::string_view tag);
  Entity GetEntity(std::string_view tag);

 private:
  friend class Entity;
  entt::registry registry_;
  std::string name_;
};
