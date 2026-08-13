#ifndef UNSTL_ALLOCATOR
#define UNSTL_ALLOCATOR

#include <concepts>
#include <cstddef>

namespace unstl {

  template <typename A>
  concept Allocator =
    requires(
      A& alloc,
      void* ptr,
      std::size_t bytes,
      std::size_t alignment
    ) {
      {
        alloc.Allocate(bytes, alignment)
      } noexcept -> std::same_as<void*>;

      {
        alloc.Deallocate(ptr, bytes, alignment)
      } noexcept;
    };

}

#endif
