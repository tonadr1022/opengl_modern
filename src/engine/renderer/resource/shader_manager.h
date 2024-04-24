#pragma once

#include <GL/glew.h>

#include <cstdint>
#include <optional>
#include <string>

#include "../gl/shader.h"

namespace gfx {
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
  static std::optional<gfx::Shader> GetShader(HashedString name);
  static std::optional<gfx::Shader> AddShader(HashedString name,
                                              const std::vector<ShaderCreateInfo>& create_info_vec);
  static std::optional<gfx::Shader> RecompileShader(HashedString name);
  static void RecompileShaders();

 private:
  struct ShaderProgramData {
    std::string name;
    GLuint id;
    std::unordered_map<uint32_t, uint32_t> uniform_locations;
    std::vector<ShaderCreateInfo> shader_create_info_vec;
  };

  static std::optional<ShaderProgramData> CompileProgram(
      HashedString name, const std::vector<ShaderCreateInfo>& create_info_vec);
  static void InitializeUniforms(ShaderProgramData& shader_program_data);

  inline static std::unordered_map<uint32_t, ShaderProgramData> shader_data_;
};
}  // namespace gfx
