#pragma once

#include <assimp/Importer.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

namespace engine {

class MaterialManager;
namespace component {
struct MeshMaterial;
}

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
  [[nodiscard]] component::MeshMaterial LoadCube();
  [[nodiscard]] std::vector<component::MeshMaterial> LoadModel(const std::string& path);
  [[nodiscard]] component::MeshMaterial LoadShape(ShapeType type);

  std::unique_ptr<Assimp::Importer> importer;

 private:
  MaterialManager& material_manager_;
  gfx::Renderer* renderer_{nullptr};
};
}  // namespace engine
