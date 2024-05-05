#include "buffer.h"

#include "engine/core/e_assert.h"
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
  EASSERT(!mapped_);
  if (id_) glDeleteBuffers(1, &id_);
}
void Buffer::Bind(GLuint target) const { glBindBuffer(target, id_); }

void Buffer::BindBase(GLuint target, GLuint slot) const { glBindBufferBase(target, slot, id_); }

void Buffer::SubData(size_t size_bytes, void* data) {
  glNamedBufferSubData(id_, offset_, size_bytes, data);
  offset_ += size_bytes;
}

void* Buffer::Map(uint32_t access) {
  mapped_ = true;
  return glMapNamedBuffer(id_, access);
}

void* Buffer::MapRange(uint32_t offset, uint32_t length_bytes, GLbitfield access) {
  return glMapNamedBufferRange(id_, offset, length_bytes, access);
}

void Buffer::Unmap() {
  mapped_ = false;
  glUnmapNamedBuffer(id_);
}

}  // namespace engine::gfx
