#include "texture_2d.h"

#include <glm/common.hpp>
#include <glm/exponential.hpp>

#include "engine/core/stb_image_impl.h"

namespace engine::gfx {
Texture2D::Texture2D(const Texture2DCreateParams& params) {
  ZoneScopedN("tex load");

  stbi_set_flip_vertically_on_load(true);

  // https://www.khronos.org/opengl/wiki/Bindless_Texture
  glCreateTextures(GL_TEXTURE_2D, 1, &id_);
  int comp;
  int x;
  int y;
  void* pixels{nullptr};
  if (!params.filepath.empty()) {
    pixels = stbi_load(params.filepath.c_str(), &x, &y, &comp, 4);
    if (pixels == nullptr) {
      spdlog::error("Failed to load texture at path {}", params.filepath);
      return;
    }
  }
  dims_.x = x;
  dims_.y = y;

  GLuint mip_levels = 0;
  if (params.generate_mipmaps) {
    mip_levels =
        static_cast<GLuint>(glm::ceil(glm::log2(static_cast<float>(glm::min(dims_.x, dims_.y)))));
  }
  // TODO(tony): separate into sampler and/or set in params
  glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, params.filter_mode_min);
  glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, params.filter_mode_max);

  // GLenum internal_format = params.s_rgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
  // specify storage
  glTextureStorage2D(id_, mip_levels, params.internal_format, dims_.x, dims_.y);
  // set the data
  glTextureSubImage2D(id_,
                      0,                 // first mip level
                      0,                 // x offset
                      0,                 // y offset
                      dims_.x,           // width
                      dims_.y,           // height
                      GL_RGBA,           // format
                      GL_UNSIGNED_BYTE,  // type
                      pixels             // data
  );
  stbi_image_free(pixels);

  if (params.generate_mipmaps) glGenerateTextureMipmap(id_);
  // https://www.khronos.org/opengl/wiki/Bindless_Texture
  if (params.bindless) {
    bindless_handle_ = glGetTextureHandleARB(id_);
    glMakeTextureHandleResidentARB(bindless_handle_);
  }
}

Texture2D::Texture2D(Texture2D&& other) noexcept
    : id_(other.id_), bindless_handle_(other.bindless_handle_), dims_(other.dims_) {}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept {
  this->id_ = std::exchange(other.id_, 0);
  this->dims_ = other.dims_;
  this->bindless_handle_ = std::exchange(other.bindless_handle_, 0);
  return *this;
}

void Texture2D::MakeNonResident() const { glMakeTextureHandleNonResidentARB(bindless_handle_); }

bool Texture2D::IsValid() const { return id_ != 0; }

Texture2D::~Texture2D() {
  glMakeTextureHandleNonResidentARB(bindless_handle_);
  glDeleteTextures(1, &id_);
}

}  // namespace engine::gfx
