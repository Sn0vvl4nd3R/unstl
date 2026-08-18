#ifndef UNSTL_STATIC_VECTOR
#define UNSTL_STATIC_VECTOR

#include "contracts.hpp"
#include <utility>
#include <cstddef>
#include <type_traits>

namespace unstl {

  template <typename Type, std::size_t Capacity>
  class StaticVector {
    private:
      alignas(Type) std::byte storage_[sizeof(Type) * Capacity];
      std::size_t size_ = 0;

      template <typename... Args>
      Type& EmplaceBackUnchecked(Args&&... args) {
        Type* ptr = ::new (PtrAt(size_)) Type(std::forward<Args>(args)...);
        ++size_;

        return *ptr;
      }

      void PopBackUnchecked(void) noexcept {
        --size_;
        PtrAt(size_)->~Type();
      }

      Type* PtrAt(std::size_t index) noexcept {
        return reinterpret_cast<Type*>(
          storage_ + index * sizeof(Type)
        );
      }

      const Type* PtrAt(std::size_t index) const noexcept {
        return reinterpret_cast<const Type*>(
          storage_ + index * sizeof(Type)
        );
      }

    public:
      StaticVector(void) noexcept = default;

      ~StaticVector(void) noexcept {
        Clear();
      }

      StaticVector(const StaticVector&) = delete;
      StaticVector& operator=(const StaticVector&) = delete;

      template <typename... Args>
      Type& EmplaceBack(Args&&... args) {
        UNSTL_EXPECT(!Full(), "Contract violation: vector is full");

        return EmplaceBackUnchecked(std::forward<Args>(args)...);
      }

      template <typename... Args>
      Type* TryEmplaceBack(Args&&... args) {
        if (Full()) [[unlikely]] {
          return nullptr;
        }

        return &EmplaceBackUnchecked(std::forward<Args>(args)...);
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

      void PopBack(void) noexcept {
        UNSTL_EXPECT(!Empty(), "Contract violation: vector is empty");

        PopBackUnchecked();
      }

      bool TryPopBack(void) noexcept {
        if (Empty()) {
          return false;
        }

        PopBackUnchecked();
        return true;
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

      Type& operator[](std::size_t index) noexcept {
        UNSTL_EXPECT(index < size_, "Contract violation: index out of bounds");

        return *PtrAt(index);
      }

      const Type& operator[](std::size_t index) const noexcept {
        UNSTL_EXPECT(index < size_, "Contract violation: index out of bounds");

        return *PtrAt(index);
      }

      Type& Front(void) noexcept {
        UNSTL_EXPECT(!Empty(), "Contract violation: vector is empty");

        return *PtrAt(0);
      }

      const Type& Front(void) const noexcept {
        UNSTL_EXPECT(!Empty(), "Contract violation: vector is empty");

        return *PtrAt(0);
      }

      Type& Back(void) noexcept {
        UNSTL_EXPECT(!Empty(), "Contract violation: vector is empty");

        return *PtrAt(size_ - 1);
      }

      const Type& Back(void) const noexcept {
        UNSTL_EXPECT(!Empty(), "Contract violation: vector is empty");

        return *PtrAt(size_ - 1);
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

      Type* Data(void) noexcept {
        return PtrAt(0);
      }

      const Type* Data(void) const noexcept {
        return PtrAt(0);
      }

      Type* Begin(void) noexcept {
        return Data();
      }

      const Type* Begin(void) const noexcept {
        return Data();
      }

      Type* End(void) noexcept {
        return Begin() + size_;
      }

      const Type* End(void) const noexcept {
        return Begin() + size_;
      }

      [[nodiscard]]
      std::size_t Size(void) const noexcept {
        return size_;
      }

      [[nodiscard]]
      static constexpr std::size_t CapacityValue(void) noexcept {
        return Capacity;
      }

      [[nodiscard]]
      bool Empty(void) const noexcept {
        return size_ == 0;
      }

      [[nodiscard]]
      bool Full(void) const noexcept {
        return size_ >= Capacity;
      }
  };

  template <typename Type, std::size_t Capacity>
  Type* begin(StaticVector<Type, Capacity>& vector) noexcept {
    return vector.Begin();
  }

  template <typename Type, std::size_t Capacity>
  const Type* begin(const StaticVector<Type, Capacity>& vector) noexcept {
    return vector.Begin();
  }

  template <typename Type, std::size_t Capacity>
  Type* end(StaticVector<Type, Capacity>& vector) noexcept {
    return vector.End();
  }

  template <typename Type, std::size_t Capacity>
  const Type* end(const StaticVector<Type, Capacity>& vector) noexcept {
    return vector.End();
  }

}

#endif
