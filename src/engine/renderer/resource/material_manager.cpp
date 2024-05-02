#include "material_manager.h"

#include "engine/renderer/material.h"

namespace gfx {

namespace {
std::unordered_map<MaterialID, Material> material_map;
uint32_t mat_counter{0};
}  // namespace

namespace material_manager {

MaterialID AddMaterial(const Material& material) {
  material_map.emplace(mat_counter, material);
  MaterialID ret = mat_counter;
  mat_counter++;
  return ret;
}

MaterialID GetMaterialHandle(HashedString name) { return name; }

Material& GetMaterial(MaterialID id) {
  auto it = material_map.find(id);
  EASSERT(it != material_map.end());
  return it->second;
}

void ClearMaterials() {
  material_map.clear();
  mat_counter = 0;
}

}  // namespace material_manager
}  // namespace gfx
