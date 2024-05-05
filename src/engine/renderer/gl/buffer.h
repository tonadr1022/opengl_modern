#pragma once

#include "engine/pch.h"

namespace engine::gfx {
enum class BufferType : uint32_t { Vertex = GL_ARRAY_BUFFER, Index = GL_ELEMENT_ARRAY_BUFFER };

class Buffer {
 public:
  Buffer() = default;
  Buffer(Buffer& other) = delete;
  Buffer& operator=(Buffer& other) = delete;
  Buffer(Buffer&& other) noexcept;
  Buffer& operator=(Buffer&& other) noexcept;

  [[nodiscard]] inline uint32_t Id() const { return id_; }
  static Buffer Create(uint32_t size_bytes, GLbitfield flags);
  void SubData(size_t size_bytes, void* data);
  void Bind(GLuint target) const;
  void BindBase(GLuint target, GLuint slot) const;
  void Reset() { offset_ = 0; };

  ~Buffer();
  [[nodiscard]] uint32_t Offset() const { return offset_; }

 private:
  uint32_t offset_{0};
  uint32_t id_{0};
};

}  // namespace engine::gfx
