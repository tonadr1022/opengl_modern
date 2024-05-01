#include "shader_manager.h"

#include <spdlog/spdlog.h>
#include <sys/resource.h>

#include "engine/util/load_file.h"

namespace gfx {

std::optional<Shader> ShaderManager::GetShader(HashedString name) {
  auto it = shader_data_.find(name);
  if (it == shader_data_.end()) {
    spdlog::error("Shader not found {}", name.data());
    return std::nullopt;
  }
  return Shader{it->second.id, it->second.uniform_locations};
}

bool CheckShaderModuleCompilationSuccess(uint32_t shader_id, const char* shaderPath) {
  int success;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    char buffer[512];
    glGetShaderInfoLog(shader_id, 512, nullptr, buffer);
    spdlog::error("Error compiling shader file: {}\n{}", shaderPath, buffer);
    return false;
  }
  return true;
}

bool CheckProgramLinkSuccess(GLuint id) {
  glLinkProgram(id);
  int success;
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    char buffer[512];
    glGetProgramInfoLog(id, 512, nullptr, buffer);
    spdlog::error("Shader Link error {}", buffer);
    return false;
  }
  return true;
}

uint32_t CompileShader(ShaderType type, const char* src) {
  uint32_t id = glCreateShader(static_cast<GLenum>(type));
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);
  return id;
}

std::optional<ShaderManager::ShaderProgramData> ShaderManager::CompileProgram(
    HashedString name, const std::vector<ShaderCreateInfo>& create_info_vec) {
  std::vector<uint32_t> shader_ids;
  for (const auto& create_info : create_info_vec) {
    auto src = util::LoadFromFile(create_info.shaderPath);
    if (!src.has_value()) {
      spdlog::error("Failed to load from file {}", create_info.shaderPath);
      return std::nullopt;
    }
    uint32_t shader_id = CompileShader(create_info.shaderType, src.value().c_str());
    if (!CheckShaderModuleCompilationSuccess(shader_id, create_info.shaderPath.c_str())) {
      return std::nullopt;
    }
    shader_ids.push_back(shader_id);
  }

  uint32_t program_id = glCreateProgram();
  for (auto& shader_id : shader_ids) {
    glAttachShader(program_id, shader_id);
  }
  glLinkProgram(program_id);
  if (!CheckProgramLinkSuccess(program_id)) {
    return std::nullopt;
  }

  ShaderProgramData data;
  data.id = program_id;
  data.name = name.data();
  data.shader_create_info_vec = create_info_vec;
  InitializeUniforms(data);
  return data;
}

std::optional<Shader> ShaderManager::AddShader(
    HashedString name, const std::vector<ShaderCreateInfo>& create_info_vec) {
  auto result = ShaderManager::CompileProgram(name, create_info_vec);
  if (!result.has_value()) {
    return std::nullopt;
  }
  spdlog::info("Compiled shader: {}", name.data());
  shader_data_.emplace(name, result.value());
  return Shader{result.value().id, result.value().uniform_locations};
}

void ShaderManager::InitializeUniforms(ShaderProgramData& program_data) {
  EASSERT_MSG(program_data.id != 0, "Can't initialize uniforms on invalid shader");
  GLint active_uniform_max_length;
  glGetProgramiv(program_data.id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &active_uniform_max_length);
  GLint num_uniforms;
  glGetProgramiv(program_data.id, GL_ACTIVE_UNIFORMS, &num_uniforms);

  GLenum uniform_type;
  GLint uniform_size;
  GLint uniform_name_length;
  char uniform_name[active_uniform_max_length];
  for (size_t i = 0; i < num_uniforms; i++) {
    glGetActiveUniform(program_data.id, i, active_uniform_max_length, &uniform_name_length,
                       &uniform_size, &uniform_type, uniform_name);
    uint32_t location = glGetUniformLocation(program_data.id, uniform_name);
    program_data.uniform_locations.emplace(HashedString(uniform_name), location);
  }
}

std::optional<Shader> ShaderManager::RecompileShader(HashedString name) {
  auto it = shader_data_.find(name);
  if (it == shader_data_.end()) {
    spdlog::warn("Shader not found, cannot recompile: {}", name.data());
    return std::nullopt;
  }
  auto recompile_result = CompileProgram(name, it->second.shader_create_info_vec);
  if (!recompile_result.has_value()) {
    return std::nullopt;
  }
  spdlog::info("Shader recompiled: {}", name.data());
  shader_data_.erase(it);
  auto new_it = shader_data_.emplace(name, recompile_result.value());
  return Shader{new_it.first->second.id, new_it.first->second.uniform_locations};
}

void ShaderManager::RecompileShaders() {
  // have to avoid iterator invalidation
  std::vector<HashedString> shader_names;
  shader_names.reserve(shader_data_.size());
  for (auto& shader_data : shader_data_) {
    shader_names.emplace_back(shader_data.second.name.data());
  }
  for (const auto& shader_name : shader_names) {
    RecompileShader(shader_name);
  }
}

}  // namespace gfx
