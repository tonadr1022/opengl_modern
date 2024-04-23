#include "shader_manager.h"

#include <spdlog/spdlog.h>

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>

#include "../e_assert.h"

std::optional<std::string> LoadFromFile(const std::string& path) {
  std::ifstream file_stream(path);
  std::string line;
  std::stringstream s_stream;
  if (!file_stream.is_open()) return std::nullopt;
  while (std::getline(file_stream, line)) {
    s_stream << line;
  }
  return s_stream.str();
}

std::optional<Shader> ShaderManager::GetShader(HashedString name) {
  auto it = shader_data_.find(name);
  if (it == shader_data_.end()) {
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
    auto src = LoadFromFile(create_info.shaderPath);
    if (!src.has_value()) {
      spdlog::error("Failed to load from file {}", create_info.shaderPath);
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
  shader_data_.emplace(result.value());
  return Shader{result.value().id, result.value().uniform_locations};
}

void ShaderManager::InitializeUniforms(ShaderProgramData& program_data) {
  DEBUG_ASSERT_MSG(program_data.id != 0, "Can't initialize uniforms on invalid shader");
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
    program_data.uniform_locations.emplace(HashedString(uniform_name), program_data.id);
  }
}
