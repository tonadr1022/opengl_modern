#pragma once
#include <entt/entt.hpp>
#include <string>

class Scene {
 public:
  explicit Scene(std::string name);
  [[nodiscard]] const std::string& GetName() const { return name_; }
  std::string name_;

 private:
  entt::registry registry_;
};
