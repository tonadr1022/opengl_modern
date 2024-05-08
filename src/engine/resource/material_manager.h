#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "engine/renderer/gl/texture_2d.h"
#include "engine/renderer/material.h"
#include "engine/resource/data_types.h"

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
  explicit MaterialManager(gfx::Renderer& renderer);
  void Init();
  [[nodiscard]] MaterialID AddMaterial(const MaterialCreateInfo& material);
  gfx::MaterialData& GetMaterial(MaterialID id);
  void RemoveMaterial(MaterialID id);
  inline MaterialID GetDefaultMaterialId() const { return default_material_id_; };
  void ClearAll();
  [[nodiscard]] std::vector<std::pair<MaterialID, gfx::MaterialData>> GetAllMaterials() const;

 private:
  MaterialID default_material_id_;
  gfx::Renderer& renderer_;
  std::unordered_map<std::string, std::unique_ptr<gfx::Texture2D>> texture_map_;
  std::unordered_map<MaterialID, gfx::MaterialData> material_map_;
  uint32_t material_counter_{0};
};

}  // namespace engine
