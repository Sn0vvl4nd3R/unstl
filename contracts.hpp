#ifndef UNSTL_CONTRACTS_HPP
#define UNSTL_CONTRACTS_HPP

#ifndef NDEGUB
  #define UNSTL_EXPECT(condition, message) \
    if (!(condition)) {                    \
      __builtin_trap();                    \
    }                                      \

#else
  #define UNSTL_EXPECT(condition, message) \
    if (!(condition)) {                    \
      __builtin_unreachable();             \
    }                                      \

#endif

#endif
