#pragma once
#include <entt/entt.hpp>
#include <string>

class Scene {
 public:
  explicit Scene(const std::string& name);
  [[nodiscard]] const std::string& GetName() const { return name_; }

 private:
  entt::registry registry_;
  std::string name_;
};
