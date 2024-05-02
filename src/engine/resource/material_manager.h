#pragma once

#include "engine/pch.h"
#include "engine/resource/data_types.h"

namespace engine {

struct Vertex;
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

}  // namespace engine
