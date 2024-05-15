#pragma once

#include "engine/core/base.h"
namespace engine {

struct Mesh {
  AssetHandle mesh_handle;
  AssetHandle material_handle;
};

struct ModelData {
  std::vector<Mesh> meshes;
};

}  // namespace engine
