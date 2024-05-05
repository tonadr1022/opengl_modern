#pragma once

#include "engine/pch.h"

struct VertexArray {
  VertexArray() = default;
  ~VertexArray();
  VertexArray(VertexArray& other) = delete;
  VertexArray& operator=(VertexArray& other) = delete;
  VertexArray(VertexArray&& other) noexcept;
  VertexArray& operator=(VertexArray&& other) noexcept;
  void Bind() const;

  [[nodiscard]] inline uint32_t Id() const { return id_; }
  static VertexArray Create();
  void AttachVertexBuffer(uint32_t buffer_id, uint32_t binding_index, uint32_t offset,
                          size_t size) const;
  void AttachElementBuffer(uint32_t buffer_id) const;

  template <typename T>
  void EnableAttribute(size_t index, size_t size, uint32_t relative_offset) const {
    glEnableVertexArrayAttrib(id_, index);

    GLenum type;
    if constexpr (std::is_floating_point_v<T>) {
      type = GL_FLOAT;
    } else if constexpr (std::is_integral_v<T>) {
      if constexpr (std::is_signed_v<T>) {
        type = GL_INT;
      } else {
        type = GL_UNSIGNED_INT;
      }
    }
    glVertexArrayAttribFormat(id_, index, size, type, GL_FALSE, relative_offset);
    glVertexArrayAttribBinding(id_, index, 0);
  }

 private:
  uint32_t id_{0};
};
