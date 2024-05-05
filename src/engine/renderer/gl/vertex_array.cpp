#include "vertex_array.h"

#include "engine/pch.h"

VertexArray VertexArray::Create() {
  VertexArray vao{};
  glCreateVertexArrays(1, &vao.id_);
  return vao;
}

VertexArray::VertexArray(VertexArray&& other) noexcept { *this = std::move(other); }

void VertexArray::Bind() const { glBindVertexArray(id_); }
VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
  if (&other == this) return *this;
  this->~VertexArray();
  id_ = std::exchange(other.id_, 0);
  return *this;
}

VertexArray::~VertexArray() {
  if (id_) glDeleteVertexArrays(1, &id_);
}

void VertexArray::AttachVertexBuffer(uint32_t buffer_id, uint32_t binding_index, uint32_t offset,
                                     size_t size) const {
  glVertexArrayVertexBuffer(id_, binding_index, buffer_id, offset, size);
}

void VertexArray::AttachElementBuffer(uint32_t buffer_id) const {
  glVertexArrayElementBuffer(id_, buffer_id);
}

void VertexArray::EnableAttributeInternal(size_t index, size_t size, uint32_t relative_offset,
                                          uint32_t type) const {
  glEnableVertexArrayAttrib(id_, index);
  glVertexArrayAttribFormat(id_, index, size, type, GL_FALSE, relative_offset);
  glVertexArrayAttribBinding(id_, index, 0);
}
