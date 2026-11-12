#ifndef BP_TEST_COMMON_COOKIE_H
#define BP_TEST_COMMON_COOKIE_H

#include <stdint.h>
#include <stdlib.h> // Just used for `size_t`.

/**
 * @brief A Test Cookie is just an abstract stateless piece of data which optionally supports 
 * the below behaviors.
 */
typedef struct bp_test_cookie_impl_t {
    /**
     * @brief Generate a string representation of a cookie.
     *
     * This is an opitonal function. When not provided, the raw value of `cookie` may be used
     * to generate a string value.
     *
     * @param[in] cookie    The cookie. Always Non-NULL.
     * @param[in] n         The size of the buffer. This function will write at most `n-1`
     *                      significant characters. This function will write a NULL terminator
     *                      after all significant characters. This function will ALWAYS write
     *                      a NULL terminator, UNLESS `n == 0`. In which case, this function
     *                      should do nothing.
     * @param[out] buf      A buffer of size at least `n`. The string representation of `cookie`
     *                      will be written here. Always Non-NULL.
     * 
     * @return Number of characters written to buf excluding the NULL terminator.
     */
    size_t (*bp_test_step_cookie_to_str)(const void *cookie, size_t n, char *buf);

    /**
     * @brief Hash function for the cookie.
     *
     * This is an optional function. When not provided, the raw value of `cookie` should be used
     * to generate a hash value for the cookie.
     *
     * @param[in] cookie    The cookie. Always Non-NULL.
     *
     * @return A 64-bit hash value derrived from the cookie's semantic value.
     */
    uint64_t (*bp_test_step_cookie_hash)(const void *cookie);

    /**
     * @brief Equality function for the cookie.
     *
     * This is an optional function. When not provided, a raw value comparision of `cookie0` and
     * `cookie1` should be used to determine equality.
     *
     * @param[in] cookie0   A cookie. Always Non-NULL.
     * @param[in] cookie1   Another cookie. Always Non-NULL.
     *
     * @return `true` if and only if `cookie0` and `cookie1` are semantically equivelant.
     */
    bool (*bp_test_step_cookie_equals)(const void *cookie0, const void *cookie1);
} bp_test_cookie_impl_t;

/**
 * @breif The set of default cookie behaviors to use when a cookie implementation (or part
 * of a cookie implenetaiton) is not provided.
 */
extern const bp_test_cookie_impl_t BP_TEST_COMMON_COOKIE_DEFAULT_IMPL;

#endif
