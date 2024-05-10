#include "shader.h"

#include <glm/gtc/type_ptr.hpp>

#include "engine/pch.h"

namespace engine::gfx {
void Shader::Bind() const {
  EASSERT_MSG(id_ != 0, "Shader is invalid");
  glUseProgram(id_);
}

void Shader::Unbind() { glUseProgram(0); }

void Shader::SetInt(entt::hashed_string name, int value) {
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
  glUniform1i(uniform_locations_[name], value);
}

void Shader::SetFloat(entt::hashed_string name, float value) {
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
  glUniform1f(uniform_locations_[name], value);
}

void Shader::SetMat4(entt::hashed_string name, const glm::mat4& mat) {
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
  glUniformMatrix4fv(uniform_locations_[name], 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetIVec2(entt::hashed_string name, const glm::ivec2& vec) {
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
  glUniform2i(uniform_locations_[name], vec[0], vec[1]);
}

void Shader::SetIVec3(entt::hashed_string name, const glm::ivec3& vec) {
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
  glUniform3iv(uniform_locations_[name], 1, glm::value_ptr(vec));
}

void Shader::SetVec3(entt::hashed_string name, const glm::vec3& vec) {
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
  glUniform3fv(uniform_locations_[name], 1, glm::value_ptr(vec));
}
void Shader::SetVec4(entt::hashed_string name, const glm::vec4& vec) {
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
  glUniform4fv(uniform_locations_[name], 1, glm::value_ptr(vec));
}

void Shader::SetMat3(entt::hashed_string name, const glm::mat3& mat, bool transpose) {
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
  glUniformMatrix3fv(uniform_locations_[name], 1, static_cast<GLboolean>(transpose),
                     glm::value_ptr(mat));
}

void Shader::SetBool(entt::hashed_string name, bool value) {
  EASSERT_MSG(uniform_locations_.contains(name), "Uniform name not found");
  glUniform1i(uniform_locations_[name], static_cast<GLint>(value));
}

Shader::Shader(uint32_t id, std::unordered_map<uint32_t, uint32_t>& uniform_locations)
    : id_(id), uniform_locations_(uniform_locations) {}

}  // namespace engine::gfx
