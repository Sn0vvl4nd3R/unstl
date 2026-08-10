#pragma once

#include "contracts.hpp"
#include "allocator.hpp"
#include <cstdint>
#include <cstddef>

namespace unstl {

  class ArenaAllocator {
    private:
      std::byte* buffer_;
      std::size_t capacity_;
      std::size_t offset_;

    public:
      ArenaAllocator(std::byte* memory, std::size_t bytes)
        : buffer_(memory), capacity_(bytes), offset_(0) {
          UNSTL_EXPECT(memory != nullptr || bytes == 0, "Invalid buffer");
      }

      ArenaAllocator(const ArenaAllocator&) = delete;
      ArenaAllocator& operator=(const ArenaAllocator&) = delete;

      [[nodiscard]]
      void* Allocate(
        std::size_t bytes,
        std::size_t alignment = alignof(std::max_align_t)
      ) noexcept {
        UNSTL_EXPECT(alignment != 0 && (alignment & (alignment - 1)) == 0, "Alignment must be a power of 2");
        std::byte* current = buffer_ + offset_;
        std::uintptr_t current_addr = reinterpret_cast<std::uintptr_t>(current);
        std::size_t padding = (-current_addr) & (alignment - 1);
        std::size_t remaining = capacity_ - offset_;
        UNSTL_EXPECT(
          padding <= remaining &&
          bytes <= remaining - padding,
          "Insufficient remaining space for allocating including padding"
        );

        offset_ += padding;
        void* result = buffer_ + offset_;
        offset_ += bytes;

        return result;
      }

      [[nodiscard]]
      void* TryAllocate(
        std::size_t bytes,
        std::size_t alignment = alignof(std::max_align_t)
      ) noexcept {
        UNSTL_EXPECT(alignment != 0 && (alignment & (alignment - 1)) == 0, "Alignment must be a power of 2");
        std::byte* current = buffer_ + offset_;
        std::uintptr_t current_addr = reinterpret_cast<std::uintptr_t>(current);
        std::size_t padding = (-current_addr) & (alignment - 1);
        std::size_t remaining = capacity_ - offset_;
        if ((padding > remaining) || (bytes > remaining - padding)) {
          return nullptr;
        }

        offset_ += padding;
        void* result = buffer_ + offset_;
        offset_ += bytes;

        return result;
      }

      void Deallocate(
        void* ptr,
        std::size_t bytes,
        std::size_t alignment
      ) noexcept {}

      void Reset(void) noexcept {
        offset_ = 0;
      }

      using marker = std::size_t;

      [[nodiscard]] marker Mark(void) const noexcept {
        return offset_;
      }
      void Rewind(marker m) noexcept {
        UNSTL_EXPECT(m <= offset_, "Marker must not exceed current offset");
        offset_ = m;
      }

      [[nodiscard]] std::size_t Remaining(void) const noexcept {
        return capacity_ - offset_;
      }

      [[nodiscard]] std::size_t Used(void) const noexcept {
        return offset_;
      }
      [[nodiscard]] std::size_t Capacity(void) const noexcept {
        return capacity_;
      }
  };

  static_assert(unstl::Allocator<ArenaAllocator>,
      "ArenaAllocator does not satisfy the unstl::Allocator concept!");
}
