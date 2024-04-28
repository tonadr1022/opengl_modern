#pragma once

#include "engine/pch.h"

namespace gfx {

using MeshID = uint32_t;

enum class ShapeType { Cube };

namespace mesh_manager {

[[nodiscard]] extern std::optional<MeshID> LoadModel(const std::string& path);
[[nodiscard]] extern MeshID LoadShape(ShapeType type);

}  // namespace mesh_manager

}  // namespace gfx
