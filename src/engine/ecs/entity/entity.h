#pragma once

#include <entt/entity/entity.hpp>

class Scene;

class Entity {
 public:
  explicit Entity(Scene& scene);

  template <typename T, typename... Args>
  T& AddComponent(Args&&... args) {}

  template <typename T>
  T GetComponent();

 private:
  Scene& scene_;
};
