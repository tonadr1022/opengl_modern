#pragma once

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

  // TODO(tony): make an enum class and a separate index into an array with actual gl values
  template <typename T>
  void EnableAttribute(size_t index, size_t size, uint32_t relative_offset) {
    uint32_t type;
    if constexpr (std::is_floating_point_v<T>) {
      type = 5126;  // GL_FLOAT
    } else if constexpr (std::is_integral_v<T>) {
      if constexpr (std::is_signed_v<T>) {
        type = 5124;  // GL_INT
      } else {
        type = 5125;  // GL_UNSIGNED_INT
      }
    }
    EnableAttributeInternal(index, size, relative_offset, type);
  }

 private:
  uint32_t id_{0};
  void EnableAttributeInternal(size_t index, size_t size, uint32_t relative_offset,
                               uint32_t type) const;
};
