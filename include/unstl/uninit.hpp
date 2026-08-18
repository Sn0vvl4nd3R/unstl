#ifndef UNSTL_UNINIT
#define UNSTL_UNINIT

#include "contracts.hpp"
#include <cstddef>
#include <utility>
#include <new>

namespace unstl {

  template <typename Type>
  class Uninit {
    private:
      alignas(Type) std::byte storage_[sizeof(Type)];

      #ifndef NDEBUG
        bool is_constructed_ = false;
      #endif

    public:
      Uninit(void) noexcept = default;
      ~Uninit(void) noexcept = default;

      Uninit(const Uninit&) = delete;
      Uninit& operator=(const Uninit&) = delete;

      [[nodiscard]]
      Type* Ptr(void) noexcept {
        return reinterpret_cast<Type*>(storage_);
      }

      [[nodiscard]]
      const Type* Ptr(void) const noexcept {
        return reinterpret_cast<const Type*>(storage_);
      }

      template <typename... Args>
      Type* Construct(Args&&... args) {
        #ifndef NDEBUG
        UNSTL_EXPECT(!is_constructed_, "Double construct!");
        is_constructed_ = true;
        #endif

        Type* ptr = ::new(Ptr()) Type(std::forward<Args>(args)...);
        return ptr;
      }

      void Destroy(void) noexcept {
        #ifndef NDEBUG
        UNSTL_EXPECT(is_constructed_, "Destroying unconstructed object!");
        is_constructed_ = false;
        #endif
        Ptr()->~Type();
      }
  };

}

#endif
