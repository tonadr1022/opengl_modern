#pragma once

#include <unistd.h>

#include <cstdint>
#include <glm/fwd.hpp>

#include "../../util/hashed_string.h"
namespace gfx {
class Shader {
 public:
  void Bind() const;
  static void Unbind();

  void SetInt(HashedString name, int value);
  void SetFloat(HashedString name, float value);
  void SetMat4(HashedString name, const glm::mat4& mat);
  void SetIVec2(HashedString name, const glm::ivec2& vec);
  void SetIVec3(HashedString name, const glm::ivec3& vec);
  void SetVec3(HashedString name, const glm::vec3& vec);
  void SetVec4(HashedString name, const glm::vec4& vec);
  void SetMat3(HashedString name, const glm::mat3& mat, bool transpose = false);
  void SetBool(HashedString name, bool value);

  Shader(uint32_t id, std::unordered_map<uint32_t, uint32_t>& uniform_locations);
  ~Shader();
  [[nodiscard]] inline uint32_t Id() const { return id_; }

 private:
  uint32_t id_{0};
  std::unordered_map<uint32_t, uint32_t>& uniform_locations_;
};

}  // namespace gfx
