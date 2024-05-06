#pragma once

#include <cstdint>
#include <glm/vec2.hpp>
#include <string>

namespace engine::gfx {

struct Texture2DCreateParams {
  std::string path;
  bool s_rgb;
  bool generate_mipmaps;
  bool bindless;
};

class Texture2D {
 public:
  explicit Texture2D(const Texture2DCreateParams& params);
  ~Texture2D();
  [[nodiscard]] uint32_t Id() const { return id_; }
  [[nodiscard]] glm::ivec2 Dims() const { return dims_; }
  [[nodiscard]] uint64_t BindlessHandle() const { return bindless_handle_; }
  void MakeNonResident() const;

 private:
  glm::ivec2 dims_{};
  uint32_t id_{0};
  uint64_t bindless_handle_{0};
};

}  // namespace engine::gfx
