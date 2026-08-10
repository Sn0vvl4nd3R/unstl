#pragma once

#include <concepts>
#include <cstddef>

namespace unstl {

  template <typename A>
  concept allocator =
    requires(
      A& alloc,
      void* ptr,
      std::size_t bytes,
      std::size_t alignment
    ) {
      { alloc.allocate(bytes, alignment) } -> std::same_as<void*>;

      {
        alloc.deallocate(ptr, bytes, alignment)
      } noexcept;
    };

}
