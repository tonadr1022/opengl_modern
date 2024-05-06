#include "material_manager.h"

#include "engine/renderer/gl/texture_2d.h"
#include "engine/renderer/material.h"
#include "engine/resource/data_types.h"

namespace engine {

MaterialID MaterialManager::AddMaterial(const MaterialCreateInfo& material) {
  gfx::Texture2DCreateParams p;
  p.bindless = true;
  p.path = material.albedo_path;
  p.generate_mipmaps = true;
  p.s_rgb = true;
  auto* tex = new gfx::Texture2D(p);
  Material m;
  m.base_color = material.base_color;
  m.albedo_map = tex->Id();

  // material_map_.emplace(mat_counter_, material);
  // MaterialID ret = mat_counter_;
  // mat_counter_++;
  // return ret;
  return 0;
}

Material& MaterialManager::GetMaterial(MaterialID id) {
  auto it = material_map_.find(id);
  EASSERT(it != material_map_.end());
  return it->second;
}

void MaterialManager::ClearMaterials() {
  material_map_.clear();
  material_counter_ = 0;
}

}  // namespace engine
