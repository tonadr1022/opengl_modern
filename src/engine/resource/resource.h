#pragma once

#include "engine/core/base.h"
namespace engine {

struct Mesh {
  AssetHandle mesh_handle;
  AssetHandle material_handle;
};

struct ModelLoadParams {
  std::string filepath;
  bool flip_textures;
};

struct ModelData {
  std::vector<Mesh> meshes;
};

}  // namespace engine
