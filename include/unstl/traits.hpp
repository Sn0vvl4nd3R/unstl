#ifndef UNSTL_TYPE_TRAITS
#define UNSTL_TYPE_TRAITS

#include <type_traits>
#include <cstring>
#include <utility>

namespace unstl {

  template <typename Type>
  struct IsRelocatable {
    static constexpr bool value = std::is_trivially_copyable_v<Type>;
  };

  template <typename Type>
  inline constexpr bool is_relocatable_v = IsRelocatable<Type>::value;

  template <typename Type>
  void RelocateUninit(Type* from, Type* to, std::size_t count) {
    if constexpr (is_relocatable_v<Type>) {
      std::memmove(to, from, count * sizeof(Type));
    } else {
      for (std::size_t i = 0; i < count; ++i) {
        ::new (static_cast<void*>(to + i)) Type(std::move(from[i]));
        from[i].~Type();
      }
    }
  }

}

#endif
