#pragma once

#include <entt/core/hashed_string.hpp>

#include "engine/pch.h"
#include "engine/renderer/gl/shader.h"

namespace engine {

enum class ShaderType {
  Vertex = GL_VERTEX_SHADER,
  Geometry = GL_GEOMETRY_SHADER,
  Fragment = GL_FRAGMENT_SHADER
};

struct ShaderCreateInfo {
  std::string shaderPath;
  ShaderType shaderType;
};

class ShaderManager {
 public:
  static std::optional<gfx::Shader> GetShader(entt::hashed_string name);
  static std::optional<gfx::Shader> AddShader(entt::hashed_string name,
                                              const std::vector<ShaderCreateInfo>& create_info_vec);
  static std::optional<gfx::Shader> RecompileShader(entt::hashed_string name);
  static void RecompileShaders();

 private:
  struct ShaderProgramData {
    std::string name;
    GLuint program_id;
    std::unordered_map<uint32_t, uint32_t> uniform_locations;
    std::vector<ShaderCreateInfo> create_info_vec;
  };

  static std::optional<ShaderProgramData> CompileProgram(
      entt::hashed_string name, const std::vector<ShaderCreateInfo>& create_info_vec);
  static void InitializeUniforms(ShaderProgramData& shader_program_data);

  inline static std::unordered_map<uint32_t, ShaderProgramData> shader_data_;
};
}  // namespace engine
