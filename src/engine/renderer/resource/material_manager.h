#pragma once

#include "engine/pch.h"
#include "engine/renderer/data_types.h"
#include "engine/renderer/material.h"

namespace gfx {
struct MaterialData;

class MaterialManager {
 public:
  [[nodiscard]] static MaterialID AddMaterial(const MaterialData& material);
  static MaterialData& GetMaterial(MaterialID id);
  static void ClearMaterials();

 private:
  static std::unordered_map<MaterialID, MaterialData> material_map_;
  static uint32_t mat_counter_;
};

}  // namespace gfx
