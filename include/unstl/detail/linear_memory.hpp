#ifndef UNSTL_LINEAR_MEMORY
#define UNSTL_LINEAR_MEMORY

#include "../contracts.hpp"
#include <cstdint>
#include <cstddef>

namespace unstl {
  namespace detail {

    struct LinearMemoryResource {
      std::byte* buffer_;
      std::size_t capacity_;
      std::size_t offset_;

      LinearMemoryResource(std::byte* memory, std::size_t bytes) noexcept
        : buffer_(memory), capacity_(bytes), offset_(0) {
        UNSTL_EXPECT(memory != nullptr || bytes == 0, "Invalid buffer");
      }

      [[nodiscard]]
      __attribute__((always_inline))
      inline void* Allocate(
        std::size_t bytes,
        std::size_t alignment
      ) noexcept {
        void* ptr = TryAllocate(bytes, alignment);
        UNSTL_EXPECT(ptr != nullptr, "Insufficient remaining space in linear buffer");
        return ptr;
      }

      [[nodiscard]]
      __attribute__((always_inline))
      inline void* TryAllocate(
        std::size_t bytes,
        std::size_t alignment
      ) noexcept {
        UNSTL_EXPECT(alignment != 0 && (alignment & (alignment - 1)) == 0, "Alignment must be a power of 2");
        std::byte* current = buffer_ + offset_;
        std::uintptr_t current_addr = reinterpret_cast<std::uintptr_t>(current);
        std::size_t padding = (-current_addr) & (alignment - 1);
        std::size_t remaining = capacity_ - offset_;

        if (padding > remaining || bytes > remaining - padding) {
          return nullptr;
        }

        offset_ += padding;
        void* result = buffer_ + offset_;
        offset_ += bytes;

        return result;
      }

    };

  }
}

#endif
