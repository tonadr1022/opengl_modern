#pragma once

#include "engine/pch.h"
#include "engine/renderer/material.h"

namespace gfx {
using MaterialType = std::variant<ColorMaterial, MaterialCreateInfo>;

namespace material_manager {

extern MaterialID AddMaterial(HashedString name, const MaterialType& create_info);
extern MaterialID AddMaterial(const MaterialType& create_info);
extern MaterialID GetMaterial(HashedString name);
extern MaterialType& GetMaterialCreateInfo(HashedString name);
extern MaterialType& GetMaterialCreateInfo(MaterialID id);
extern void ClearMaterials();

}  // namespace material_manager

}  // namespace gfx
