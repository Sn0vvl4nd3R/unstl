#ifndef UNSTL_POOL_ALLOCATOR
#define UNSTL_POOL_ALLOCATOR

#include "contracts.hpp"
#include "allocator.hpp"
#include <cstddef>
#include <cstdint>

namespace unstl {

  class PoolAllocator {
    private:
      struct FreeNode {
        FreeNode* next;
      };

      std::byte* buffer_;
      std::size_t capacity_;
      std::size_t chunk_size_;
      std::size_t offset_;
      FreeNode* head_;

    public:
      PoolAllocator(std::byte* memory, std::size_t bytes, std::size_t chunk_size) noexcept
        : buffer_(memory), capacity_(bytes), chunk_size_(chunk_size), offset_(0), head_(nullptr) {
        UNSTL_EXPECT(memory != nullptr || bytes == 0, "Invalid buffer");
        UNSTL_EXPECT(chunk_size_ >= sizeof(FreeNode), "Chunk size too small");
      }

      PoolAllocator(const PoolAllocator&) = delete;
      PoolAllocator& operator=(const PoolAllocator&) = delete;

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
      ) noexcept {
        if (bytes == 0 || bytes > chunk_size_) {
          return nullptr;
        }
        if (head_ != nullptr) {
          if (reinterpret_cast<std::uintptr_t>(head_) % alignment != 0) {
            return nullptr;
          }
          FreeNode* node = head_;
          head_ = head_->next;
          return static_cast<void*>(node);
        }

        std::uintptr_t current_addr = reinterpret_cast<std::uintptr_t>(buffer_ + offset_);
        std::uintptr_t aligned_addr = (current_addr + (alignment - 1)) & ~(alignment - 1);
        std::size_t padding = aligned_addr - current_addr;

        if (offset_ + padding + chunk_size_ > capacity_) {
          return nullptr;
        }

        offset_ += padding + chunk_size_;
        return reinterpret_cast<void*>(aligned_addr);
      }

      void Deallocate(
        void* ptr,
        std::size_t bytes,
        std::size_t alignment
      ) noexcept {
        if (ptr == nullptr) {
          return;
        }
        
        UNSTL_EXPECT(
          static_cast<std::byte*>(ptr) >= buffer_ &&
          static_cast<std::byte*>(ptr) < buffer_ + capacity_,
          "Pointer is outside pool buffer"
        );
        UNSTL_EXPECT(bytes <= chunk_size_, "Deallocated size exceeds chunk size");
        UNSTL_EXPECT(
          reinterpret_cast<std::uintptr_t>(ptr) % alignment == 0,
          "Pointer alignment mismatch"
        );

        FreeNode* node = static_cast<FreeNode*>(ptr);
        node->next = head_;
        head_ = node;
      }

      void Reset(void) noexcept {
        offset_ = 0;
        head_ = nullptr;
      }
  };

  static_assert(unstl::Allocator<PoolAllocator>,
    "PoolAllocator does not satisfy the unstl::Allocator concept!");
}

#endif
