#pragma once

#include "contracts.hpp"
#include "allocator.hpp"
#include <cstdint>
#include <cstddef>

namespace unstl {

  class arena_allocator {
    private:
      std::byte* buffer_;
      std::size_t capacity_;
      std::size_t offset_;

    public:
      arena_allocator(std::byte* memory, std::size_t bytes)
        : buffer_(memory), capacity_(bytes), offset_(0) {
          UNSTL_EXPECT(memory != nullptr || bytes == 0, "Invalid buffer");
      }

      arena_allocator(const arena_allocator&) = delete;
      arena_allocator& operator=(const arena_allocator&) = delete;

      [[nodiscard]]
      void* allocate(
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
      void* try_allocate(
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

      void deallocate(
        void* ptr,
        std::size_t bytes,
        std::size_t alignment
      ) noexcept {}

      void reset(void) noexcept {
        offset_ = 0;
      }

      using marker = std::size_t;

      [[nodiscard]] marker mark(void) const noexcept {
        return offset_;
      }
      void rewind(marker m) noexcept {
        UNSTL_EXPECT(m <= offset_, "Marker must not exceed current offset");
        offset_ = m;
      }

      [[nodiscard]] std::size_t remaining(void) const noexcept {
        return capacity_ - offset_;
      }

      [[nodiscard]] std::size_t used(void) const noexcept {
        return offset_;
      }
      [[nodiscard]] std::size_t capacity(void) const noexcept {
        return capacity_;
      }
  };

  static_assert(unstl::allocator<arena_allocator>,
      "arena_allocator does not satisfy the unstl::allocator concept!");
}
