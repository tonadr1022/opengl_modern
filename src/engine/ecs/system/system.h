#pragma once

#include <entt/fwd.hpp>

namespace engine {
struct Timestep;

struct System {
  virtual void Update(entt::registry& registry, Timestep timestep);
};

}  // namespace engine
