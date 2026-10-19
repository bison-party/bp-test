#ifndef BP_TEST_COMMON_SEQ_H 
#define BP_TEST_COMMON_SEQ_H

#include <stdarg.h>
#include <setjmp.h>
#include <stdint.h>

typedef uint8_t bp_test_log_level_t;

#define BP_T_LL_ERROR 0
#define BP_T_LL_WARN 1
#define BP_T_LL_INFO 2
#define BP_T_LL_DEBUG 3

/**
 * @brief The result of a single test step
 */
typedef uint8_t bp_test_step_status_t;

/**
 * @brief A test step succeeded. Test execution should continue to the next step.
 */
#define BP_T_SS_CONTINUE 0

/**
 * @brief A test step succeeded. The test does not need to continue with following steps.
 */
#define BP_T_SS_EARLY_PASS 1

/**
 * @brief A test step failed. The test should not continue with following steps.
 */
#define BP_T_SS_ERROR 2

typedef struct bp_test_context_t bp_test_context_t;
typedef struct bp_test_context_impl_t bp_test_context_impl_t;

typedef struct bp_test_step_t bp_test_step_t;

struct bp_test_step_t {
    /**
     * @brief Name of the test step.
     */
    const char *name;

    /**
     * @brief Extra test step information to be passed to the test at runtime.
     *
     * This is an optional field. When used, it is meant to be a stateless value.
     */
    const void *cookie;

    /**
     * @brief The test step behavior. 
     */
    bp_test_step_status_t (*bp_test_step_fp)(bp_test_context_t *ctx, const void *cookie);
};


typedef struct bp_test_seq_t bp_test_seq_t;



#ifndef BP_TEST_COMMON_SEQ_MAX_CAP
/**
 * @brief Static maximum of the number of steps in a single test sequence.
 */
#define BP_TEST_COMMON_SEQ_MAX_CAP 16
#endif

struct bp_test_seq_t {

};

#ifndef BP_TEST_COMMON_CTX_MAP_CAP
/**
 * @brief Static capacity of test context map.
 */
#define BP_TEST_COMMON_CTX_MAP_CAP 32
#endif

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

    /**
     * @brief jump buffer used for exiting test from arbitrary position.
     */
    jmp_buf jb;
    
    const bp_test_context_impl_t * const impl;
};

struct bp_test_context_impl_t {
    void (*bp_tc_log)(bp_test_context_t *ctx, bp_test_log_level_t ll, unsigned int line, const char *func, const char *msg);
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
}

#endif
