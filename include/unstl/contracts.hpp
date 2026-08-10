#pragma once

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
