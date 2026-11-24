#ifndef BP_TEST_COMMON_SEQ_H 
#define BP_TEST_COMMON_SEQ_H

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h> // Just used for `size_t`.

#include "test-common/deps.h"
#include "test-common/cookie.h"

typedef uint8_t bp_test_log_level_t;

#define BP_TEST_LOG_LEVEL_ERROR 0
#define BP_TEST_LOG_LEVEL_WARN 1
#define BP_TEST_LOG_LEVEL_INFO 2
#define BP_TEST_LOG_LEVEL_DEBUG 3

/**
 * @brief How a test sequence should behave after a step completes.
 *
 * This is an `int` to confirm to return code argument taken by `longjmp`.
 */
typedef int bp_test_step_action_t;

/**
 * @brief A test step succeeded. Test execution should continue to the next step.
 */
#define BP_TEST_STEP_ACTION_CONTINUE 1

/**
 * @brief A test step succeeded. The test does not need to continue with following steps.
 */
#define BP_TEST_STEP_ACTION_EARLY_PASS 2

/**
 * @brief A test step failed. The test should not continue with following steps.
 */
#define BP_TEST_STEP_ACTION_FAIL 3

typedef struct bp_test_context_t bp_test_context_t;

/**
 * @brief A single test step.
 */
typedef struct bp_test_step_t {
    /**
     * @brief Name of the test step.
     */
    const char *name;

    /**
     * @brief An optional cookie to use for this step.
     */
    const void *cookie;

    /**
     * @brief An optional cookie implementation for `cookie`.
     *
     * If `cookie` is provided, but `cookie_impl` is not, this is OK! In this case, whichever
     * endpoints are not provided will be substituted with those found in 
     * `BP_TEST_COMMON_COOKIE_DEFAULT_IMPL`.
     *
     * If `cookie_impl` is provided, `cookie` __MUST__ be Non-NULL.
     */
    const bp_test_cookie_impl_t *cookie_impl;

    /**
     * @brief The test step.
     *
     * @param[in] ctx       The test context.
     * @param[in] cookie    The stateless cookie.
     *
     * The actual test step behavior must __ONLY__ either accept no cookie at all, or 
     * cookies of the same type!
     *
     * Formally, if `step_a->bp_test_step == step_b->bp_test_step` then
     * `step_a->cookie_impl == step_b->cookie_impl`.
     *
     * @return Action code describing what to do after this step completes.
     */
    bp_test_step_action_t (*bp_test_step)(bp_test_context_t *ctx, const void *cookie);
} bp_test_step_t;

/**
 * @brief Compute the hash of a test step.
 *
 * @param test_step The test step to hash. Always Non-NULL.
 *
 * @return A hash value which is a function of the test step's handler and its cookie
 * (optionally).
 */
uint32_t bp_test_step_hash(const bp_test_step_t *test_step);

/**
 * @brief compare two test steps for equality.
 *
 * @param test_step0 A test step. Always Non-NULL.
 * @param test_step1 Another test step. Always Non-NULL.
 *
 * @return `true` iff both test steps are semantically equal.
 */
bool bp_test_step_equals(const bp_test_step_t *test_step0, const bp_test_step_t *test_step1);


#ifndef BP_TEST_COMMON_CTX_MAP_CAP
/**
 * @brief Static capacity of test context map.
 */
#define BP_TEST_COMMON_CTX_MAP_CAP 32
#endif

typedef struct bp_test_context_impl_t bp_test_context_impl_t;

/**
 * @brief The context passed between test steps during test execution.
 */
struct bp_test_context_t {
    /**
     * @brief Test context vtable.
     */
    const bp_test_context_impl_t * const impl;

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
         * Non-NULL if and only if this pair is occupied.
         */
        const char *key;

        /**
         * @brief An arbitrary pointer. 
         *
         * Non-NULL if and only if this pair is occupied.
         */
        void *value;
    } map[BP_TEST_COMMON_CTX_MAP_CAP];

    /**
     * @brief jump buffer used for exiting test from arbitrary position.
     */
    jmp_buf jb;
};

struct bp_test_context_impl_t {
    /**
     * This function is entirely OPTIONAL.
     *
     * @see `bp_test_context_deinit`
     */
    void (*deinit)(bp_test_context_t *ctx);

    /**
     * @brief Most fundamental logging function for the test step.
     *
     * This function should only exit the test if there is an error actually preforming
     * the log (even in this case, exiting is optional). For example, if `ll` is `BP_T_LL_ERROR`,
     * this endpoint should just log the message just like with any other log level.
     * Wrappers around this endpoint will perform early test exiting logic as needed. 
     * For this reason, this endpoint should __NEVER__ be called directly from a test step!
     * Always use wrapper macros!
     * 
     * This function is entirely OPTIONAL.
     *
     * @param ctx   This test context. Always Non-NULL.
     * @param ll    The log level.
     * @param line  The line number of the log call.
     * @param func  The name of the function containing the line call. Always Non-NULL.
     * @param msg   The message to log. Always Non-NULL.
     */
    void (*log)(bp_test_context_t *ctx, bp_test_log_level_t ll, unsigned int line, const char *func, const char *msg);
};

/**
 * @brief Initialize the `bp_test_context` base class fields.
 *
 * @param ctx   The context to initialize. Always Non-NULL.
 * @param impl  The context vtable. Always Non-NULL. Must exist unmodified for the life time
 *              of `ctx`.
 *
 * @warning     This leaves the jump buffer `jb` in an undefined state.
 */
void bp_test_context_base_init(bp_test_context_t *ctx, const bp_test_context_impl_t *impl);

/**
 * @brief Deinitialize the context.
 *
 * @param ctx   The context to deinitialize. Always Non-NULL.
 */
void bp_test_context_deinit(bp_test_context_t *ctx);

/**
 * @brief Store a key value pair inside the context's static map.
 *
 * @param ctx   The context. Always Non-NULL.
 * @param key   The string key. 
 * @param value The value.
 *
 * If either `key` or `value` are NULL, this function will explicitly ignore the put request.
 *
 * @warning `key` and `value` MUST have indefinite lifetimes! A reference to a local variable
 *          of a single step should NEVER be put into the context map.
 *
 * @return True if and only if `key` was put into the map.
 */
bool bp_test_context_put(bp_test_context_t *ctx, const char *key, void *value);

/**
 * @brief Remove a key from the context's static map.
 *
 * @param ctx   The context. Always Non-NULL.
 * @param key   The key to remove. 
 *
 * If `key` is `NULL`, `false` is returned immediately.
 *
 * @return `true` if and only if `key` is found in the map. If so, it is gauranteed to be
 * removed.
 */
bool bp_test_context_remove(bp_test_context_t *ctx, const char *key);

/**
 * @brief Get a value from the context's static map.
 *
 * @param ctx   The context. Always Non-NULL.
 * @param key   The key to lookup.
 *
 * @return The value associated with `key`. `NULL` if `key` is not in the map.
 */
void *bp_test_context_get(bp_test_context_t *ctx, const char *key);

/**
 * @brief Declare return point for a test step.
 *
 * @param ctx   The context.
 *
 * @warning     This is a macro around `setjmp` and thus can only be used in expressions/conditions.
 */
#define bp_test_context_setjmp(ctx) \
    setjmp((ctx)->jb)

/**
 * @brief Leave a test step early.
 *
 * @param ctx       The test context.
 * @param action    The action to perform after leaving.
 */
void bp_test_context_leave(bp_test_context_t *ctx, bp_test_step_action_t action);

static inline void bp_test_context_continue(bp_test_context_t *ctx) {
    bp_test_context_leave(ctx, BP_TEST_STEP_ACTION_CONTINUE);
}

static inline void bp_test_context_early_pass(bp_test_context_t *ctx) {
    bp_test_context_leave(ctx, BP_TEST_STEP_ACTION_EARLY_PASS);
}

static inline void bp_test_context_fail(bp_test_context_t *ctx) {
    bp_test_context_leave(ctx, BP_TEST_STEP_ACTION_FAIL);
}

static inline void bp_test_context_log(bp_test_context_t *ctx, bp_test_log_level_t ll, unsigned int line, const char *func, const char *msg) {
    ctx->impl->log(ctx, ll, line, func, msg);
}

#ifndef BP_TEST_COMMON_LOG_BUFFER_CAP
/**
 * @brief The static buffer size of test logging functions.
 */
#define BP_TEST_COMMON_LOG_BUFFER_CAP 256
#endif

#if BP_TEST_COMMON_LOG_BUFFER_CAP == 0
#error "BP Test requires non-zero buffer capacity"
#endif

/**
 * @brief Variadic wrapper around `bp_test_context_t::log`.
 *
 * It is advised that you always use helper macros below for logging!
 *
 * @warning If `ll` is `BP_TEST_LOG_LEVEL_ERROR` this will exit the test early!
 */
void bp_test_context_logf(bp_test_context_t *ctx, bp_test_log_level_t ll, unsigned int line, const char *func, const char *fmt, ...);

/*
 * NOTE: Log calls for ALL log levels are ALWAYS compiled in.
 *
 * Depending on how the virtual log endpoint is implemented, messaged can be ignored dynamically
 * instead of at compile time.
 */

#define BP_TEST_ERROR(ctx, fmt, ...) \
    bp_test_context_logf(ctx, BP_TEST_LOG_LEVEL_ERROR, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define BP_TEST_WARN(ctx, fmt, ...) \
    bp_test_context_logf(ctx, BP_TEST_LOG_LEVEL_WARN, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define BP_TEST_INFO(ctx, fmt, ...) \
    bp_test_context_logf(ctx, BP_TEST_LOG_LEVEL_INFO, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define BP_TEST_DEBUG(ctx, fmt, ...) \
    bp_test_context_logf(ctx, BP_TEST_LOG_LEVEL_DEBUG, __LINE__, __func__, fmt, ##__VA_ARGS__)


typedef struct bp_test_seq_t {
    /**
     * @brief Name of the full test sequence.
     */
    const char *name; 

    /**
     * @brief A NULL terminated table of pointers to the steps of this sequence.
     */
    const bp_test_step_t * const *steps;
} bp_test_seq_t;

/**
 * @brief Compute the hash of a test sequence.
 *
 * @param test_seq The test sequence to hash. Always Non-NULL.
 *
 * @return A combination of the hash value of all steps.
 */
uint32_t bp_test_seq_hash(const bp_test_seq_t *test_seq);

/**
 * @brief Compare two test sequences for equality.
 *
 * @param test_seq0 A test sequence. Always Non-NULL.
 * @param test_seq1 Another test sequence. Always Non-NULL.
 * 
 * @return Whether or not `test_seq0` is semantically equal to `test_seq1`.
 */
bool bp_test_seq_equals(const bp_test_seq_t *test_seq0, const bp_test_seq_t *test_seq1);

#endif
