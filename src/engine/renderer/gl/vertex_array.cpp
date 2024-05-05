#include "vertex_array.h"

VertexArray VertexArray::Create() {
  VertexArray vao{};
  glCreateVertexArrays(1, &vao.id_);
  return vao;
}

VertexArray::VertexArray(VertexArray&& other) noexcept : id_(other.id_) { other.id_ = 0; }

void VertexArray::Bind() const {}
VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
  std::exchange(this->id_, other.id_);
  other.id_ = 0;
  return *this;
}

VertexArray::~VertexArray() { glDeleteVertexArrays(1, &id_); }

void VertexArray::AttachVertexBuffer(uint32_t buffer_id, uint32_t binding_index, uint32_t offset,
                                     size_t size) const {
  glVertexArrayVertexBuffer(id_, binding_index, buffer_id, offset, size);
}

void VertexArray::AttachElementBuffer(uint32_t buffer_id) const {
  glVertexArrayElementBuffer(id_, buffer_id);
}
