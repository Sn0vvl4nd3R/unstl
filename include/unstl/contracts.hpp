#ifndef UNSTL_CONTRACTS_HPP
#define UNSTL_CONTRACTS_HPP

#ifndef NDEBUG

#define UNSTL_EXPECT(condition, message) \
    do {                                 \
        if (!(condition)) {              \
            __builtin_trap();            \
        }                                \
    } while (false)

#else

#define UNSTL_EXPECT(condition, message) \
    do {                                 \
        if (!(condition)) {              \
            __builtin_unreachable();     \
        }                                \
    } while (false)

#endif

#endif
