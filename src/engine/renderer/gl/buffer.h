#pragma once

#include <cstdint>

namespace engine::gfx {

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
  void Bind(uint32_t target) const;
  void BindBase(uint32_t target, uint32_t slot) const;
  void Reset() { offset_ = 0; };
  void* Map(uint32_t access);
  void* MapRange(uint32_t offset, uint32_t length_bytes, GLbitfield access);
  void Unmap();

  ~Buffer();
  [[nodiscard]] uint32_t Offset() const { return offset_; }

 private:
  uint32_t offset_{0};
  uint32_t id_{0};
  bool mapped_{false};
};

}  // namespace engine::gfx
