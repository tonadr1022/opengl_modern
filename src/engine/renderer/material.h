#pragma once

#include <initializer_list>

#include "../pch.h"
#include "engine/util/hashed_string.h"
#include "gl/shader.h"

namespace gfx {

struct MaterialData {
  HashedString shader_id;
  glm::vec3 diffuse;
  glm::vec3 roughness_metallic;
};

}  // namespace gfx
