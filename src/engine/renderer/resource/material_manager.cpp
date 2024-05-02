#include "material_manager.h"

#include "engine/renderer/material.h"

namespace gfx {

std::unordered_map<MaterialID, MaterialData> MaterialManager::material_map_;
uint32_t MaterialManager::mat_counter_{0};

MaterialID MaterialManager::AddMaterial(const MaterialData& material) {
  material_map_.emplace(mat_counter_, material);
  MaterialID ret = mat_counter_;
  mat_counter_++;
  return ret;
}

MaterialData& MaterialManager::GetMaterial(MaterialID id) {
  auto it = material_map_.find(id);
  EASSERT(it != material_map_.end());
  return it->second;
}

void MaterialManager::ClearMaterials() {
  material_map_.clear();
  mat_counter_ = 0;
}

}  // namespace gfx
