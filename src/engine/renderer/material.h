#pragma once

#include <initializer_list>

#include "../pch.h"
#include "engine/util/hashed_string.h"
#include "gl/shader.h"

namespace gfx {

using MaterialID = uint32_t;

struct CustomUniformData {
  HashedString id;
  std::function<void(HashedString id, Shader& shader)> SetterFunction;
};

struct MaterialAttribute {
  // @brief the type of uniform
  enum class Type { Vec3, Vec4, Float, Uint } type;
  // @brief the name of the uniform to set
  HashedString name;
  // @bried value of the uniform
  using Value = union {
    glm::vec3 vec3_v;
    glm::vec4 vec4_v;
    float float_v;
    uint32_t uint_v;
  };
  Value value;

  MaterialAttribute(Type type, HashedString name, Value value)
      : type(type), name(name), value(value) {}
};

struct Material {
  HashedString shader_id;
  std::vector<MaterialAttribute> attributes;
  // TODO(tony): need texture info
  std::vector<CustomUniformData> uniform_data;
};

}  // namespace gfx
