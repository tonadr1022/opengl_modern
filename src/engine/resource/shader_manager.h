#include <GL/glew.h>

#include <cstdint>
#include <optional>
#include <string>

#include "../renderer/gl/shader.h"

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
  static std::optional<Shader> GetShader(HashedString name);
  static std::optional<Shader> LoadShader(HashedString name);
  static std::optional<Shader> RecompileShader(HashedString name);
  static void RecompileShaders();

 private:
  struct ShaderProgramData {
    std::string name;
    GLuint id;
    std::unordered_map<uint32_t, uint32_t>& uniform_locations;
    std::vector<ShaderCreateInfo> shader_create_info_vec;
  };

  static std::optional<Shader> CompileProgram();

  inline static std::unordered_map<uint32_t, ShaderProgramData> shader_data_;
};
