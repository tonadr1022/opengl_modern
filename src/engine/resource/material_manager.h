#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "engine/core/base.h"
#include "engine/renderer/gl/texture_2d.h"
#include "engine/renderer/material.h"

namespace engine {

struct Vertex;
namespace gfx {
class Renderer;
}

struct MaterialCreateInfo {
  glm::vec3 base_color{1, 1, 1};
  std::optional<std::string> albedo_path;
  std::optional<std::string> roughness_path;
  std::optional<std::string> metalness_path;
  std::optional<std::string> ao_path;
  std::optional<std::string> normal_path;
};

class MaterialManager {
 public:
  static MaterialManager& Get();
  void Init();
  [[nodiscard]] AssetHandle AddMaterial(const MaterialCreateInfo& material);
  gfx::MaterialData& GetMaterial(AssetHandle material_handle);
  void RemoveMaterial(AssetHandle id);
  inline AssetHandle GetDefaultMaterialHandle() const { return default_material_handle_; };
  void Reset();
  [[nodiscard]] std::vector<std::pair<AssetHandle, gfx::MaterialData>> GetAllMaterials() const;

 private:
  // static class only to be created once by engine
  friend class Engine;
  MaterialManager();
  static MaterialManager* instance_;

  AssetHandle default_material_handle_;

  std::unordered_map<std::string, std::unique_ptr<gfx::Texture2D>> texture_map_;
  std::unordered_map<AssetHandle, gfx::MaterialData> material_map_;
};

}  // namespace engine
