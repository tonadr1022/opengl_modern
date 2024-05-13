#pragma once

#include <cstdint>
#include <glm/vec2.hpp>
#include <string>

namespace engine::gfx {

struct TextureCubeCreateParams {
  std::string filepath;
  // uint32_t internal_format;
  // bool generate_mipmaps{true};
  bool bindless{true};
  // uint32_t filter_mode_min;
  // uint32_t filter_mode_max;
};

class TextureCube {
 public:
  explicit TextureCube(const TextureCubeCreateParams& params);
  TextureCube(const TextureCube& other) = delete;
  TextureCube operator=(const TextureCube& other) = delete;
  TextureCube(TextureCube&& other) noexcept;
  TextureCube& operator=(TextureCube&& other) noexcept;
  ~TextureCube();
  [[nodiscard]] uint32_t Id() const { return id_; }
  [[nodiscard]] glm::ivec2 Dims() const { return dims_; }
  [[nodiscard]] uint64_t BindlessHandle() const { return bindless_handle_; }
  void MakeNonResident() const;
  [[nodiscard]] bool IsValid() const;

 private:
  glm::ivec2 dims_{};
  uint32_t id_{0};
  uint64_t bindless_handle_{0};
};

}  // namespace engine::gfx
