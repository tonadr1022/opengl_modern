#pragma once

#include "engine/renderer/material.h"
#include "engine/renderer/resource/mesh_manager.h"

namespace component {

struct Mesh {
  gfx::MeshID handle;
};

struct Material {
  gfx::MaterialID handle;
};

}  // namespace component
