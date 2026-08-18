#ifndef UNSTL_STATIC_VECTOR
#define UNSTL_STATIC_VECTOR

#include "contracts.hpp"
#include "uninit.hpp"
#include <cstddef>

namespace unstl {

  template <typename Type, std::size_t Capacity>
  class StaticVector {
    private:
      Uninit<Type> storage_[Capacity];
      std::size_t size_ = 0;

      template <typename... Args>
      Type& EmplaceBackUnchecked(Args&&... args) {
        Type& value = *storage_[size_].Construct(std::forward<Args>(args)...);
        size_++;

        return value;
      }

      void PopBackUnchecked(void) noexcept {
        --size_;
        storage_[size_].Destroy();
      }

    public:
      StaticVector(void) noexcept = default;

      ~StaticVector(void) noexcept {
        for (std::size_t i = 0; i < size_; ++i) {
          storage_[i].Destroy();
        }
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
        if (Full()) {
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
        while (!Empty()) {
          PopBackUnchecked();
        }
      }

      Type& operator[](std::size_t index) noexcept {
        UNSTL_EXPECT(index < size_, "Contract violation: index out of bounds");

        return *storage_[index].Ptr();
      }

      const Type& operator[](std::size_t index) const noexcept {
        UNSTL_EXPECT(index < size_, "Contract violation: index out of bounds");

        return *storage_[index].Ptr();
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
        return size_ == Capacity;
      }
  };

}

#endif
