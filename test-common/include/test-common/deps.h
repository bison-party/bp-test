#ifndef BP_TEST_COMMON_DEPS_H
#define BP_TEST_COMMON_DEPS_H

#include <stdlib.h> // just for size_t
#include <stdarg.h>

/**
 * @brief `vsnprintf` implementation used by bp-test-common.
 *
 * @param[out] buf  A writeable buffer with size at least `n`. Always Non-NULL.
 * @param[in] n     The size of the buffer.
 * @param[in] fmt   The format string to use.
 * @param[in] vargs The variadic args list.
 *
 * @return Number of characters written to `buf` excluding the NULL terminator. On error,
 * a negative value is returned.
 *
 * This should behave like normal `vsnprintf`. However, I learned that the `libc` formatting 
 * functions are allowed to use `malloc` under the hood. This is potentially problematic if
 * working in an environment without `malloc` (Or you are testing `malloc` itself).
 */
extern int bp_test_vsnprintf(char *buf, size_t n, const char *fmt, va_list vargs);

/**
 * @brief Varargs wrapper around `bp_test_vsnprintf`.
 */
int bp_test_snprintf(char *buf, size_t n, const char *fmt, ...);

#endif
