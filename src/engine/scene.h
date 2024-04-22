#pragma once

#include <entt/entt.hpp>
#include <string>

class Scene {
 public:
  explicit Scene(const std::string& name);

 private:
  entt::registry registry_;
  std::string name_;
};
