#pragma once

#include <unordered_map>
#include <vector>

#include "engine/renderer/material.h"
#include "engine/resource/data_types.h"

namespace engine {

struct Vertex;

struct MaterialCreateInfo {
  glm::vec3 base_color{1, 1, 1};
  std::string albedo_path;
};

class MaterialManager {
 public:
  [[nodiscard]] MaterialID AddMaterial(const MaterialCreateInfo& material);
  Material& GetMaterial(MaterialID id);
  void ClearMaterials();

 private:
  std::vector<Material> materials_;
  std::unordered_map<MaterialID, Material> material_map_;
  uint32_t material_counter_{0};
};

}  // namespace engine
