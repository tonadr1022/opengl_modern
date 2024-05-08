#include "texture_2d.h"

#include <filesystem>

#include "engine/core/stb_image_impl.h"
#include "engine/pch.h"

namespace engine::gfx {
Texture2D::Texture2D(const Texture2DCreateParams& params) {
  ZoneScopedN("tex load");
  bool has_tex =
      std::filesystem::exists(params.path) && std::filesystem::is_regular_file(params.path);
  if (!has_tex) {
    spdlog::error("no texture at path: {}", params.path);
    return;
  }
  stbi_set_flip_vertically_on_load(true);

  // https://www.khronos.org/opengl/wiki/Bindless_Texture
  glCreateTextures(GL_TEXTURE_2D, 1, &id_);
  int comp;
  int x;
  int y;
  void* pixels = stbi_load(params.path.c_str(), &x, &y, &comp, 4);
  dims_.x = x;
  dims_.y = y;
  EASSERT_MSG(pixels != nullptr, "Failed to load texture");

  GLuint mip_levels = 1;
  if (params.generate_mipmaps) {
    mip_levels =
        static_cast<GLuint>(glm::ceil(glm::log2(static_cast<float>(glm::min(dims_.x, dims_.y)))));
  }
  // TODO(tony): separate into sampler and/or set in params
  glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLenum internal_format = params.s_rgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
  // specify storage
  glTextureStorage2D(id_, mip_levels, internal_format, dims_.x, dims_.y);
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
  bindless_handle_ = glGetTextureHandleARB(id_);
  glMakeTextureHandleResidentARB(bindless_handle_);
}

void Texture2D::MakeNonResident() const { glMakeTextureHandleNonResidentARB(bindless_handle_); }

Texture2D::~Texture2D() {
  glMakeTextureHandleNonResidentARB(bindless_handle_);
  glDeleteTextures(1, &id_);
}

}  // namespace engine::gfx
