#pragma once

#include <cstdint>
#include <entt/core/hashed_string.hpp>
#include <glm/fwd.hpp>
#include <unordered_map>

namespace engine::gfx {

// lightweight, meant to be created and destroyed every frame, contains an int and a reference.
class Shader {
 public:
  void Bind() const;
  static void Unbind();

  void SetInt(entt::hashed_string name, int value);
  void SetFloat(entt::hashed_string name, float value);
  void SetMat4(entt::hashed_string name, const glm::mat4& mat);
  void SetIVec2(entt::hashed_string name, const glm::ivec2& vec);
  void SetIVec3(entt::hashed_string name, const glm::ivec3& vec);
  void SetVec3(entt::hashed_string name, const glm::vec3& vec);
  void SetVec4(entt::hashed_string name, const glm::vec4& vec);
  void SetMat3(entt::hashed_string name, const glm::mat3& mat, bool transpose = false);
  void SetBool(entt::hashed_string name, bool value);

  Shader(uint32_t id, std::unordered_map<uint32_t, uint32_t>& uniform_locations);
  ~Shader() = default;
  [[nodiscard]] inline uint32_t Id() const { return id_; }

 private:
  uint32_t id_{0};
  std::unordered_map<uint32_t, uint32_t>& uniform_locations_;
};

}  // namespace engine::gfx
