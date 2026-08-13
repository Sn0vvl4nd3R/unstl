#ifndef UNSTL_POOL_ALLOCATOR
#define UNSTL_POOL_ALLOCATOR

#include "contracts.hpp"
#include "allocator.hpp"
#include <cstddef>
#include <cstdint>

namespace unstl {

  class PoolAllocator {
    private:

    public:
      [[nodiscard]]
      void* Allocate(
        std::size_t bytes,
        std::size_t alignment = alignof(std::max_align_t)
      ) noexcept {
        void* ptr = TryAllocate(bytes, alignment);
        UNSTL_EXPECT(ptr != nullptr, "Pool memory exhausted");
        return ptr;
      }

      [[nodiscard]]
      void* TryAllocate(
        std::size_t bytes,
        std::size_t alignment = alignof(std::max_align_t)
      ) noexcept {}

      void Deallocate(
        void* ptr,
        std::size_t bytes,
        std::size_t alignment
      ) noexcept {

      }
  };

  static_assert(unstl::Allocator<PoolAllocator>,
    "PoolAllocator does not satisfy the unstl::Allocator concept!");
}

#endif
