#include "buffer.h"

namespace engine::gfx {

Buffer Buffer::Create(uint32_t size_bytes, GLbitfield flags) {
  Buffer buffer{};
  glCreateBuffers(1, &buffer.id_);
  glNamedBufferStorage(buffer.id_, size_bytes, nullptr, flags);
  return buffer;
}

Buffer::~Buffer() {
  spdlog::critical("deleting buffer {}", id_);
  glDeleteBuffers(1, &id_);
  exit(1);
}
void Buffer::Bind(GLuint target) const { glBindBuffer(target, id_); }

void Buffer::BindBase(GLuint target, GLuint slot) const {
  glBindBufferBase(target, slot, id_);
  glBindBuffer(target, id_);
}

void Buffer::SubData(size_t size_bytes, void* data) {
  glNamedBufferSubData(id_, offset_, size_bytes, data);
  offset_ += size_bytes;
}

}  // namespace engine::gfx
