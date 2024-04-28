#pragma once

#include "engine/pch.h"

namespace gfx {
enum class BufferType : uint32_t { Vertex = GL_ARRAY_BUFFER, Index = GL_ELEMENT_ARRAY_BUFFER };

// class Buffer {
//  public:
//   [[nodiscard]] inline uint32_t GetId() const { return id_; }
//   void Create(uint32_t size);
//
//  private:
//   uint32_t id_{0};
// };

}  // namespace gfx
