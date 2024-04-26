#include "material_manager.h"

#include "engine/renderer/material.h"

namespace gfx {

namespace {
std::unordered_map<MaterialID, MaterialType> material_map;
uint32_t counter{0};

}  // namespace

namespace material_manager {

MaterialID AddMaterial(HashedString name, const MaterialType& create_info) {
  material_map.emplace(name, create_info);
  return name;
}

MaterialID AddMaterial(const MaterialType& create_info) {
  material_map.emplace(counter, create_info);
  MaterialID ret = counter;
  counter++;
  return ret;
}

MaterialID GetMaterial(HashedString name) { return name; }

MaterialType& GetMaterialCreateInfo(HashedString name) {
  auto it = material_map.find(name);
  EASSERT(it != material_map.end());
  return it->second;
}

void ClearMaterials() {
  material_map.clear();
  counter = 0;
}

}  // namespace material_manager
}  // namespace gfx
