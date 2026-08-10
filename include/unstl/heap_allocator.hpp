#pragma once

#include "contracts.hpp"
#include "allocator.hpp"
#include <stdlib.h>

namespace unstl {

  class heap_allocator {
    public:
      [[nodiscard]]
      void* allocate(
        std::size_t bytes,
        std::size_t alignment = alignof(std::max_align_t)
      ) noexcept {
        UNSTL_EXPECT(alignment != 0 && (alignment & (alignment - 1)) == 0, "Alignment must be a power of 2");
        void* ptr = try_allocate(bytes, alignment);
        UNSTL_EXPECT(ptr != nullptr, "Failed to allocate memory");
        return ptr;
      }

      [[nodiscard]]
      void* try_allocate(
        std::size_t bytes,
        std::size_t alignment = alignof(std::max_align_t)
      ) noexcept {
        UNSTL_EXPECT(alignment != 0 && (alignment & (alignment - 1)) == 0, "Alignment must be a power of 2");
        UNSTL_EXPECT((bytes % alignment) == 0, "Size must be a multiple of alignment");
        return std::aligned_alloc(alignment, bytes);
      }

      void deallocate(
        void* ptr,
        std::size_t bytes,
        std::size_t alignment
      ) noexcept {
        free(ptr);
      }
  };

  static_assert(unstl::allocator<heap_allocator>,
      "heap_allocator does not satisfy the unstl::allocator concept!");
}
