#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace engine {

using MaterialID = uint32_t;
using MeshID = uint32_t;
using Index = uint32_t;

namespace gfx {
class Renderer;
}
enum class ShapeType { Cube };

class MeshManager {
 public:
  void Init(gfx::Renderer* renderer);
  MeshID LoadCube();
  [[nodiscard]] std::optional<MeshID> LoadModel(const std::string& path);
  [[nodiscard]] MeshID LoadShape(ShapeType type);

 private:
  gfx::Renderer* renderer_{nullptr};
};
}  // namespace engine
