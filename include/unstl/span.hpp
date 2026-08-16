#ifndef UNSTL_SPAN
#define UNSTL_SPAN

#include "contracts.hpp"
#include <cstddef>

namespace unstl {

  template <typename Type>
  class Span {
    private:
      Type* data_ = nullptr;
      std::size_t size_ = 0;

    public:
      Span(void) noexcept = default;

      Span(Type* data, std::size_t size) noexcept
      : data_(data), size_(size) {
        UNSTL_EXPECT(size == 0 || data != nullptr, "Contract violation: bad constructor");
      }

      template <std::size_t Size>
      Span(Type (&data)[Size]) noexcept
      : data_(data), size_(Size) {}

      [[nodiscard]]
      Type* Data(void) const noexcept {
        return data_;
      }

      [[nodiscard]]
      std::size_t Size(void) const noexcept {
        return size_;
      }

      [[nodiscard]]
      bool Empty(void) const noexcept {
        return size_ == 0;
      }

      Type& operator[](std::size_t index) const noexcept {
        UNSTL_EXPECT(index < size_, "Span index out of bounds");
        return data_[index];
      }

      [[nodiscard]]
      Type& Front(void) const noexcept {
        UNSTL_EXPECT(size_ != 0, "Span is empty");
        return data_[0];
      }

      [[nodiscard]]
      Type& Back(void) const noexcept {
        UNSTL_EXPECT(size_ != 0, "Span is empty");
        return data_[size_ - 1];
      }

      [[nodiscard]]
      Type* Begin(void) const noexcept {
        return data_;
      }

      [[nodiscard]]
      Type* End(void) const noexcept {
        return data_ + size_;
      }

      [[nodiscard]]
      Span<Type> Subspan(std::size_t offset, std::size_t count) const noexcept {
        UNSTL_EXPECT(offset <= size_, "Offset is out of bounds");
        UNSTL_EXPECT(count <= size_ - offset, "Subspan is out of bounds");

        return Span<Type>(Begin() + offset, count);
      }

      [[nodiscard]]
      Span<Type> First(std::size_t count) const noexcept {
        return Subspan(0, count);
      }

      [[nodiscard]]
      Span<Type> Last(std::size_t count) const noexcept {
        UNSTL_EXPECT(count <= size_, "Subspan is out of bounds");

        return Subspan(size_ - count, count);
      }
  };

  template <typename Type>
  Type* begin(const Span<Type>& span) noexcept {
    return span.Begin();
  }
  
  template <typename Type>
  Type* end(const Span<Type>& span) noexcept {
    return span.End();
  }

}

#endif
