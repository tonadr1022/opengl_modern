#include "shader_manager.h"

#include <optional>

std::optional<Shader> ShaderManager::GetShader(HashedString name) {
  auto it = shader_data_.find(name);
  if (it == shader_data_.end()) {
    return std::nullopt;
  }
  return Shader{it->second.id, it->second.uniform_locations};
}

std::optional<Shader> ShaderManager::LoadShader(HashedString name) { return std::nullopt; }
