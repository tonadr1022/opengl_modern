#pragma once

#include "engine/pch.h"
#include "engine/renderer/material.h"

namespace gfx {
struct Material;

namespace material_manager {

[[nodiscard]] extern MaterialID AddMaterial(HashedString name, const Material& material);
[[nodiscard]] extern MaterialID AddMaterial(const Material& material);
extern Material& GetMaterial(MaterialID id);
extern void ClearMaterials();

}  // namespace material_manager

}  // namespace gfx
