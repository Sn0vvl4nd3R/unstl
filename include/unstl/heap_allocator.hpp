#pragma once

#include "contracts.hpp"
#include "allocator.hpp"
#include <stdlib.h>

namespace unstl {

  class HeapAllocator {
    public:
      [[nodiscard]]
      void* Allocate(
        std::size_t bytes,
        std::size_t alignment = alignof(std::max_align_t)
      ) noexcept {
        UNSTL_EXPECT(alignment != 0 && (alignment & (alignment - 1)) == 0, "Alignment must be a power of 2");
        void* ptr = TryAllocate(bytes, alignment);
        UNSTL_EXPECT(ptr != nullptr, "Failed to allocate memory");
        return ptr;
      }

      [[nodiscard]]
      void* TryAllocate(
        std::size_t bytes,
        std::size_t alignment = alignof(std::max_align_t)
      ) noexcept {
        UNSTL_EXPECT(alignment != 0 && (alignment & (alignment - 1)) == 0, "Alignment must be a power of 2");
        UNSTL_EXPECT((bytes % alignment) == 0, "Size must be a multiple of alignment");
        return std::aligned_alloc(alignment, bytes);
      }

      void Deallocate(
        void* ptr,
        std::size_t bytes,
        std::size_t alignment
      ) noexcept {
        free(ptr);
      }
  };

  static_assert(unstl::Allocator<HeapAllocator>,
    "HeapAllocator does not satisfy the unstl::Allocator concept!");
}
