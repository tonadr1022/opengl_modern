#include "shader.h"

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>

#include "../../e_assert.h"

#define CHECK 1

namespace gfx {
void Shader::Bind() const {
  DEBUG_ASSERT_MSG(id_ != 0, "Shader is invalid");
  glUseProgram(id_);
}

void Shader::Unbind() { glUseProgram(0); }

void Shader::SetInt(HashedString name, int value) {
#ifdef CHECK
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
#endif
  glUniform1i(uniform_locations_[name], value);
}

void Shader::SetFloat(HashedString name, float value) {
#ifdef CHECK
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
#endif
  glUniform1f(uniform_locations_[name], value);
}

void Shader::SetMat4(HashedString name, const glm::mat4& mat) {
#ifdef CHECK
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
#endif
  glUniformMatrix4fv(uniform_locations_[name], 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetIVec2(HashedString name, const glm::ivec2& vec) {
#ifdef CHECK
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
#endif
  glUniform2i(uniform_locations_[name], vec[0], vec[1]);
}

void Shader::SetIVec3(HashedString name, const glm::ivec3& vec) {
#ifdef CHECK
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
#endif
  glUniform3iv(uniform_locations_[name], 1, glm::value_ptr(vec));
}

void Shader::SetVec3(HashedString name, const glm::vec3& vec) {
#ifdef CHECK
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
#endif
  glUniform3fv(uniform_locations_[name], 1, glm::value_ptr(vec));
}
void Shader::SetVec4(HashedString name, const glm::vec4& vec) {
#ifdef CHECK
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
#endif
  glUniform4fv(uniform_locations_[name], 1, glm::value_ptr(vec));
}

void Shader::SetMat3(HashedString name, const glm::mat3& mat, bool transpose) {
#ifdef CHECK
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
#endif
  glUniformMatrix3fv(uniform_locations_[name], 1, static_cast<GLboolean>(transpose),
                     glm::value_ptr(mat));
}

void Shader::SetBool(HashedString name, bool value) {
#ifdef CHECK
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
#endif
  glUniform1i(uniform_locations_[name], static_cast<GLint>(value));
}

Shader::Shader(uint32_t id, std::unordered_map<uint32_t, uint32_t>& uniform_locations)
    : id_(id), uniform_locations_(uniform_locations) {}

}  // namespace gfx
