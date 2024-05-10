#pragma once

#include "engine/core/base.h"
namespace engine {

namespace component {

struct Mesh {
  AssetHandle mesh_handle;
  AssetHandle material_handle;
};

}  // namespace component
}  // namespace engine
