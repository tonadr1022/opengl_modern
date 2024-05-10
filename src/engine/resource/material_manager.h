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
  static MaterialManager& Get();
  void Init();
  [[nodiscard]] MaterialHandle AddMaterial(const MaterialCreateInfo& material);
  gfx::MaterialData& GetMaterial(MaterialHandle id);
  void RemoveMaterial(MaterialHandle id);
  inline MaterialHandle GetDefaultMaterialId() const { return default_material_id_; };
  void ClearAll();
  [[nodiscard]] std::vector<std::pair<MaterialHandle, gfx::MaterialData>> GetAllMaterials() const;

 private:
  // static class only to be created once by engine
  friend class Engine;
  MaterialManager();
  static MaterialManager* instance_;

  MaterialHandle default_material_id_;

  std::unordered_map<std::string, std::unique_ptr<gfx::Texture2D>> texture_map_;
  std::unordered_map<MaterialHandle, gfx::MaterialData> material_map_;
  uint32_t material_counter_{0};
};

}  // namespace engine
