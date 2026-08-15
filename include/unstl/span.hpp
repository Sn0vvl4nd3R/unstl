#ifndef UNSTL_SPAN
#define UNSTL_SPAN

#include "contracts.hpp"
#include <cstddef>

namespace unstl {

  template <typename Type>
  class Span {
    private:
      Type* data_;
      std::size_t size_;

    public:
      Span(Type* data, std::size_t size) noexcept
      : data_(data), size_(size) {
        UNSTL_EXPECT(size == 0 || data != nullptr, "Contract violation: bad constructor");
      }

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
  };
  
}

#endif
