#include "buffer.h"

#include "engine/pch.h"

namespace engine::gfx {

Buffer Buffer::Create(uint32_t size_bytes, GLbitfield flags) {
  Buffer buffer{};
  glCreateBuffers(1, &buffer.id_);
  glNamedBufferStorage(buffer.id_, size_bytes, nullptr, flags);
  return buffer;
}

Buffer::Buffer(Buffer&& other) noexcept { *this = std::move(other); }

Buffer& Buffer::operator=(Buffer&& other) noexcept {
  if (&other == this) return *this;
  this->~Buffer();
  id_ = std::exchange(other.id_, 0);
  offset_ = std::exchange(other.offset_, 0);
  return *this;
}

Buffer::~Buffer() {
  if (id_) glDeleteBuffers(1, &id_);
}
void Buffer::Bind(GLuint target) const { glBindBuffer(target, id_); }

/** @brief ssbos only */
void Buffer::BindBase(GLuint target, GLuint slot) const {
  glBindBufferBase(target, slot, id_);
  glBindBuffer(target, id_);
}

void Buffer::SubData(size_t size_bytes, void* data) {
  glNamedBufferSubData(id_, offset_, size_bytes, data);
  offset_ += size_bytes;
}

}  // namespace engine::gfx
