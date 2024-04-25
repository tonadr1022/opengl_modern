#pragma once

#include "../pch.h"
#include "gl/shader.h"

namespace gfx {

using MaterialID = uint32_t;

struct PerMaterialUniformData {
  HashedString id;
  std::function<void(HashedString id, Shader& shader)> SetterFunction;
};

struct MaterialCreateInfo {
  HashedString shader_id;
  // need texture info
  std::vector<PerMaterialUniformData> uniform_data;
};

}  // namespace gfx
