#pragma once

namespace engine {

struct Timestep {
  double dt_actual{0};     // actual dt
  double dt_effective{0};  // effective, is 0 when paused, use for gameplay
};

}  // namespace engine
