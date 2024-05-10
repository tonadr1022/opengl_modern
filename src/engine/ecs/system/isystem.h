#pragma once

#include <entt/entity/fwd.hpp>

namespace engine::ecs {
struct ISystem {
  entt::registry& registry;
  bool enabled;
  explicit ISystem(entt::registry& reg, bool enabled = true) : registry(reg), enabled(enabled) {}
};
}  // namespace engine::ecs
