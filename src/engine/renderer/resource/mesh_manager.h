#pragma once

#include "engine/pch.h"

namespace gfx {

using MeshID = uint32_t;

namespace mesh_manager {

extern MeshID LoadModel(const std::string& path);

}  // namespace mesh_manager

}  // namespace gfx
