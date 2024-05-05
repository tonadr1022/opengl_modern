#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace engine {

using MaterialID = uint32_t;
using MeshID = uint32_t;
using Index = uint32_t;

enum class ShapeType { Cube };

class MeshManager {
 public:
  [[nodiscard]] static std::optional<MeshID> LoadModel(const std::string& path);
  [[nodiscard]] static MeshID LoadShape(ShapeType type);
};
}  // namespace engine
