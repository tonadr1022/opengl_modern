#pragma once

#include <entt/entity/entity.hpp>

namespace engine {

class Scene;

class Entity {
 public:
  Entity();

  template <typename T, typename... Args>
  T& AddComponent(Args&&... args) {}

  template <typename T>
  T GetComponent();

 private:
};

}  // namespace engine
