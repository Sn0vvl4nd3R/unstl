#pragma once

#include "contracts.hpp"
#include "allocator.hpp"
#include "detail/linear_memory.hpp"
#include <cstddef>
#include <cstdint>

namespace unstl {

  class StackAllocator {
    private:
      detail::LinearMemoryResource res_;

    public:
      StackAllocator(std::byte* memory, std::size_t bytes) noexcept
        : res_(memory, bytes) {
          UNSTL_EXPECT(memory != nullptr || bytes == 0, "Invalid buffer");
      }

      [[nodiscard]]
      void* Allocate(
        std::size_t bytes,
        std::size_t alignment = alignof(std::max_align_t)
      ) noexcept {
        return res_.Allocate(bytes, alignment);
      }

      [[nodiscard]]
      void* TryAllocate(
        std::size_t bytes,
        std::size_t alignment = alignof(std::max_align_t)
      ) noexcept {
        return res_.TryAllocate(bytes, alignment);
      }

      void Deallocate(
        void* ptr,
        std::size_t bytes,
        std::size_t alignment
      ) noexcept {
        std::byte* p = static_cast<std::byte*>(ptr);

        if (p + bytes == res_.buffer_ + res_.offset_) {
          res_.offset_ -= bytes;
        }
      }

      void Reset(void) noexcept {
        res_.offset_ = 0;
      }

      using marker = std::size_t;

      [[nodiscard]]
      marker Mark(void) const noexcept {
        return res_.offset_;
      }
      void Rewind(marker m) noexcept {
        UNSTL_EXPECT(m <= res_.offset_, "Marker must not exceed current offset");
        res_.offset_ = m;
      }

      [[nodiscard]]
      std::size_t Remaining(void) const noexcept {
        return res_.capacity_ - res_.offset_;
      }
  };

  static_assert(unstl::Allocator<StackAllocator>,
      "StackAllocator does not satisfy the unstl::Allocator concept!");
}
