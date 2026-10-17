#ifndef BP_TEST_COMMON_CTX_H 
#define BP_TEST_COMMON_CTX_H

#include <stdarg.h>

#ifndef BP_TEST_COMMON_CTX_MAP_CAP
/**
 * @brief Static capacity of test context map.
 */
#define BP_TEST_COMMON_CTX_MAP_CAP 32
#endif

typedef enum bp_test_log_level_t {
    BP_T_LL_ERROR = 0,
    BP_T_LL_WARN,
    BP_T_LL_INFO,
    BP_T_LL_DEBUG,
} bp_test_log_level_t;

typedef struct bp_test_context_t bp_test_context_t;
typedef struct bp_test_context_impl_t bp_test_context_impl_t;

struct bp_test_context_t {
    /**
     * @brief test context key value map.
     *
     * The purpose of this structure is preserve state across test steps.
     * You may want to store a value in one step, and use it in a following step.
     *
     * @warning 
     *      Keys and values in the map most persist across steps! __NEVER__ store a reference
     *      to a local variable in `value` or `key`. 
     */
    struct {
        /**
         * @brief A pointer to a string literal.
         *
         * NULL means an empty pair.
         */
        const char *key;

        /**
         * @brief An arbitrary pointer.
         */
        void *value;
    } map[BP_TEST_COMMON_CTX_MAP_CAP];
    
    const bp_test_context_impl_t * const impl;
};

struct bp_test_context_impl_t {
    /**
     * @brief see `bp_tc_logf` for details.
     */
    void (*bp_tc_logf_fp)(bp_test_context_t *ctx, bp_test_log_level_t ll, const char *fmt, va_list args);
};

/**
 * @brief logging function for use within a test.
 *
 * __NOTE__ This function should __not__ exit the test. It is just for logging messages, that's it!
 *
 * @param ctx a pointer to the test context.
 * @param ll the log level to output the message as.
 * @param fmt the format string.
 */
static inline void bp_tc_logf(bp_test_context_t *ctx, bp_test_log_level_t ll, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ctx->impl->bp_tc_logf_fp(ctx, ll, fmt, args);
    va_end(args);

    // How do we say though that we have advanced from one step to another?
    // Ever think of that one bub...?
}

#endif
