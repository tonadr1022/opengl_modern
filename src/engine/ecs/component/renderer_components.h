#pragma once

#include <cstdint>
namespace engine {
using MaterialID = uint32_t;
using MeshID = uint32_t;

namespace component {

struct Mesh {
  MeshID handle;
};

struct Material {
  MaterialID handle;
};

}  // namespace component
}  // namespace engine
