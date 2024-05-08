#pragma once

#include <entt/entity/fwd.hpp>

namespace engine::ecs {
struct ISystem {
  entt::registry* registry{nullptr};
};
}  // namespace engine::ecs
