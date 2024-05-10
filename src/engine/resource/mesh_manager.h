#pragma once

#include <assimp/Importer.hpp>
#include <cstdint>
#include <memory>
#include <string>

namespace engine {

class MaterialManager;
namespace component {
struct Mesh;
}

using MaterialHandle = uint32_t;
using MeshID = uint32_t;
using Index = uint32_t;

namespace gfx {
class Renderer;
}

enum class ShapeType { Cube };

class MeshManager {
 public:
  static MeshManager& Get();
  ~MeshManager();  // default
  void Init(gfx::Renderer* renderer);

  // [[nodiscard]] component::Mesh LoadCube();
  // [[nodiscard]] component::MeshMaterial LoadShape(ShapeType type);

  std::unique_ptr<Assimp::Importer> importer;

 private:
  // singleton only to be created once by engine
  friend class Engine;
  static MeshManager* instance_;
  MeshManager();
};
}  // namespace engine
