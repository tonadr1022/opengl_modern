#pragma once

#include <assimp/Importer.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

namespace engine {

class MaterialManager;

using MaterialID = uint32_t;
using MeshID = uint32_t;
using Index = uint32_t;

namespace gfx {
class Renderer;
}
enum class ShapeType { Cube };

class MeshManager {
 public:
  explicit MeshManager(MaterialManager& material_manager);
  void Init(gfx::Renderer* renderer);
  MeshID LoadCube();
  [[nodiscard]] std::optional<MeshID> LoadModel(const std::string& path);
  [[nodiscard]] MeshID LoadShape(ShapeType type);

  std::unique_ptr<Assimp::Importer> importer;

 private:
  MaterialManager& material_manager_;
  gfx::Renderer* renderer_{nullptr};
};
}  // namespace engine
