#ifndef UNSTL_STATIC_RING_BUFFER
#define UNSTL_STATIC_RING_BUFFER

#include "contracts.hpp"
#include <cstddef>
#include <utility>
#include <type_traits>

namespace unstl {
  
  template <typename Type, std::size_t Capacity>
  class StaticRingBuffer {
    static_assert(
      Capacity > 0,
      "StaticRingBuffer capacity must be greater than zero"
    );

    private:
      alignas(Type) std::byte storage_[sizeof(Type) * Capacity];
      std::size_t head_ = 0;
      std::size_t size_ = 0;

      std::size_t PhysicalIndex(std::size_t index) const noexcept {
        return (head_ + index) % Capacity;
      }

      Type* PtrAtPhysical(std::size_t index) noexcept {
        return reinterpret_cast<Type*>(
          storage_ + index * sizeof(Type)
        );
      }

      const Type* PtrAtPhysical(std::size_t index) const noexcept {
        return reinterpret_cast<const Type*>(
          storage_ + index * sizeof(Type)
        );
      }

      Type* PtrAt(std::size_t index) noexcept {
        return PtrAtPhysical(PhysicalIndex(index));
      }

      const Type* PtrAt(std::size_t index) const noexcept {
        return PtrAtPhysical(PhysicalIndex(index));
      }

      template <typename... Args>
      Type& EmplaceBackUnchecked(Args&&... args) {
        Type* ptr = ::new (PtrAt(size_)) Type(std::forward<Args>(args)...);
        ++size_;

        return *ptr;
      }

      template <typename... Args>
      Type& EmplaceFrontUnchecked(Args&&... args) {
        std::size_t new_head = head_ == 0 ? Capacity - 1 : head_ - 1;
        Type* ptr = ::new (PtrAtPhysical(new_head)) Type(std::forward<Args>(args)...);
        head_ = new_head;
        ++size_;

        return *ptr;
      }

      void PopFrontUnchecked(void) noexcept {
        PtrAt(0)->~Type();
        head_ = (head_ + 1) % Capacity;
        --size_;
      }

      void PopBackUnchecked(void) noexcept {
        PtrAt(size_ - 1)->~Type();
        --size_;
      }

    public:
      StaticRingBuffer(void) noexcept = default;

      StaticRingBuffer(const StaticRingBuffer&) = delete;
      StaticRingBuffer& operator=(const StaticRingBuffer&) = delete;

      StaticRingBuffer(StaticRingBuffer&&) = delete;
      StaticRingBuffer& operator=(StaticRingBuffer&&) = delete;

      ~StaticRingBuffer(void) noexcept {
        Clear();
      }

      template <typename... Args>
      Type& EmplaceBack(Args&&... args) {
        UNSTL_EXPECT(!Full(), "Contract violation: ring buffer is full");

        return EmplaceBackUnchecked(std::forward<Args>(args)...);
      }

      template <typename... Args>
      Type* TryEmplaceBack(Args&&... args) {
        if (Full()) [[unlikely]] {
          return nullptr;
        }

        return &EmplaceBackUnchecked(std::forward<Args>(args)...);
      }

      template <typename... Args>
      Type& EmplaceFront(Args&&... args) {
        UNSTL_EXPECT(!Full(), "Contract violation: ring buffer is full");

        return EmplaceFrontUnchecked(std::forward<Args>(args)...);
      }

      template <typename... Args>
      Type* TryEmplaceFront(Args&&... args) {
        if (Full()) [[unlikely]] {
          return nullptr;
        }

        return &EmplaceFrontUnchecked(std::forward<Args>(args)...);
      }

      Type& PushBack(const Type& value) {
        return EmplaceBack(value);
      }

      Type& PushBack(Type&& value) {
        return EmplaceBack(std::move(value));
      }

      Type* TryPushBack(const Type& value) {
        return TryEmplaceBack(value);
      }

      Type* TryPushBack(Type&& value) {
        return TryEmplaceBack(std::move(value));
      }

      Type& PushFront(const Type& value) {
        return EmplaceFront(value);
      }

      Type& PushFront(Type&& value) {
        return EmplaceFront(std::move(value));
      }

      Type* TryPushFront(const Type& value) {
        return TryEmplaceFront(value);
      }

      Type* TryPushFront(Type&& value) {
        return TryEmplaceFront(std::move(value));
      }

      void PopFront(void) noexcept {
        UNSTL_EXPECT(!Empty(), "Contract violation: ring buffer is empty");

        PopFrontUnchecked();
      }

      bool TryPopFront(void) noexcept {
        if (Empty()) {
          return false;
        }

        PopFrontUnchecked();
        return true;
      }

      void PopBack(void) noexcept {
        UNSTL_EXPECT(!Empty(), "Contract violation: ring buffer is empty");

        PopBackUnchecked();
      }

      bool TryPopBack(void) noexcept {
        if (Empty()) {
          return false;
        }

        PopBackUnchecked();
        return true;
      }

      Type& Front(void) noexcept {
        UNSTL_EXPECT(!Empty(), "Contract violation: ring buffer is empty");

        return *PtrAt(0);
      }

      const Type& Front(void) const noexcept {
        UNSTL_EXPECT(!Empty(), "Contract violation: ring buffer is empty");

        return *PtrAt(0);
      }

      Type* TryFront(void) noexcept {
        if (Empty()) {
          return nullptr;
        }

        return PtrAt(0);
      }

      const Type* TryFront(void) const noexcept {
        if (Empty()) {
          return nullptr;
        }

        return PtrAt(0);
      }

      Type& Back(void) noexcept {
        UNSTL_EXPECT(!Empty(), "Contract violation: ring buffer is empty");

        return *PtrAt(size_ - 1);
      }

      const Type& Back(void) const noexcept {
        UNSTL_EXPECT(!Empty(), "Contract violation: ring buffer is empty");

        return *PtrAt(size_ - 1);
      }

      Type* TryBack(void) noexcept {
        if (Empty()) {
          return nullptr;
        }

        return PtrAt(size_ - 1);
      }

      const Type* TryBack(void) const noexcept {
        if (Empty()) {
          return nullptr;
        }

        return PtrAt(size_ - 1);
      }

      Type& operator[](std::size_t index) noexcept {
        UNSTL_EXPECT(index < size_, "Contract violation: index out of range");

        return *PtrAt(index);
      }

      const Type& operator[](std::size_t index) const noexcept {
        UNSTL_EXPECT(index < size_, "Contract violation: index out of range");

        return *PtrAt(index);
      }

      void Clear(void) noexcept {
        if constexpr (std::is_trivially_destructible_v<Type>) {
          size_ = 0;
        } else {
          while (!Empty()) {
            PopBackUnchecked();
          }
        }
      }

      [[nodiscard]]
      std::size_t Size(void) const noexcept {
        return size_;
      }

      [[nodiscard]]
      std::size_t CapacityValue(void) const noexcept {
        return Capacity;
      }

      [[nodiscard]]
      bool Empty(void) const noexcept {
        return size_ == 0;
      }

      [[nodiscard]]
      bool Full(void) const noexcept {
        return size_ == Capacity;
      }
  };

}

#endif
