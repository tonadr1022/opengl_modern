#pragma once

#include "engine/pch.h"
#include "engine/renderer/data_types.h"

namespace gfx {

enum class ShapeType { Cube };

class MeshManager {
 public:
  [[nodiscard]] static std::optional<MeshID> LoadModel(const std::string& path);
  [[nodiscard]] static MeshID LoadShape(ShapeType type);
};
}  // namespace gfx
