#pragma once

#include <unordered_map>
#include <vector>

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
  static std::vector<MaterialData> materials_;
  static std::vector<MaterialID, size_t> material_index_map_;
  static std::unordered_map<MaterialID, MaterialData> material_map_;
  static uint32_t mat_counter_;
};

}  // namespace engine
