#ifndef BP_TEST_COMMON_SEQ_H 
#define BP_TEST_COMMON_SEQ_H

#include <stdarg.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h> // Just used for `size_t`.

#include "test-common/cookie.h"

typedef uint8_t bp_test_log_level_t;

#define BP_TEST_LL_ERROR 0
#define BP_TEST_LL_WARN 1
#define BP_TEST_LL_INFO 2
#define BP_TEST_LL_DEBUG 3

/**
 * @brief How a test sequence should behave after a step completes.
 */
typedef uint8_t bp_test_step_action_t;

/**
 * @brief A test step succeeded. Test execution should continue to the next step.
 */
#define BP_TEST_SA_CONTINUE 0

/**
 * @brief A test step succeeded. The test does not need to continue with following steps.
 */
#define BP_TEST_SA_EARLY_PASS 1

/**
 * @brief A test step failed. The test should not continue with following steps.
 */
#define BP_TEST_SA_FAIL 2

typedef struct bp_test_context_t bp_test_context_t;
typedef struct bp_test_context_impl_t bp_test_context_impl_t;

typedef struct bp_test_step_t bp_test_step_t;

typedef struct bp_test_seq_t bp_test_seq_t;

struct bp_test_step_t {
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
};

/**
 * @brief compute the hash of a test step.
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

struct bp_test_seq_t {
    /**
     * @brief Name of the full test sequence.
     */
    const char *name; 

    /**
     * @brief A NULL terminated table of pointers to the steps of this sequence.
     */
    const bp_test_step_t * const *steps;
};

#ifndef BP_TEST_COMMON_CTX_MAP_CAP
/**
 * @brief Static capacity of test context map.
 */
#define BP_TEST_COMMON_CTX_MAP_CAP 32
#endif


/**
 * @brief The context passed between test steps during test execution.
 *
 * A test context is meant to be the *dynamic* state of the test. It provides reflection into
 * what step is currently being executed, a key value store, and more!
 */
struct bp_test_context_t {
    /**
     * @brief The test sequence being run.
     */
    const bp_test_seq_t * const seq;

    /**
     * @brief The test step being run.
     */
    const bp_test_step_t * const step;

    /**
     * @brief Test step index for convenience.
     *
     * `step = seq->steps[step_index]`
     */
    const size_t step_index;

    /**
     * @brief Test context implementation.
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
};

struct bp_test_context_impl_t {
    // What would this even do exactly?
    // Send a message back potentially?
    // I mean, shouldn't a sequence have a unique name if the cookie of a single step is differnet?
    // If a sequnce is really a sum of its parts that is?
    // I feel like a test sequence should have a name correct?
    // Which is what exactly... the sum of the names of the steps I think?
    // Should a test sequnce have like some sort of hash for identificaiton?
    // That could be cool also right? I think potentially yes?
    // Search wouldn't be that slow IMO... Just saying...
    // I feel like that makes sense IMO.. Test output could be verbose, but also good IMO.
    // I think this could be pretty cool IMO.. Test output being hasahed could be cool.
    //
    // 
    //
    void (*bp_test_context_on_step_enter)(bp_test_context_t *ctx);

    // I feel like this is confusing shit...
    // A sequence is a set of steps... that's all really?
    // I don't really want this being overly abstract tbh...?

    /**
     * @brief Most fundamental logging function for the test step.
     *
     * This function pointer should only exit the test if there is an error actually preforming
     * the log (even in this case, exiting is optional). For example, if `ll` is `BP_T_LL_ERROR`,
     * this endpoint should just log the message just like with any other log level.
     * Wrappers around this endpoint will perform early test exiting logic as needed. 
     * For this reason, this endpoint should __NEVER__ be called directly from a test step!
     * Always use helpers!
     *
     * This function is entirely OPTIONAL.
     *
     * @param ctx   This test context.
     * @param ll    The log level.
     * @param line  The line number of the log call.
     * @param func  The name of the function containing the line call.
     * @param msg   The message to log.
     */
    void (*bp_tc_log)(bp_test_context_t *ctx, bp_test_log_level_t ll, unsigned int line, const char *func, const char *msg);

    // What about entering/exiting a sequence though? Shouldn't this be
    void (*bp_test_context_on_step_exit)(bp_test_context_t *ctx);
};


/**
 * @brief Leave a test step early.
 *
 * @param ctx The test context.
 * @param action The action to perform after leaving.
 */
void bp_test_leave_step(bp_test_context_t *ctx, bp_test_step_action_t action);

static inline void bp_test_continue(bp_test_context_t *ctx) {
    bp_test_leave_step(ctx, BP_TEST_SA_CONTINUE);
}

static inline void bp_test_early_pass(bp_test_context_t *ctx) {
    bp_test_leave_step(ctx, BP_TEST_SA_EARLY_PASS);
}

static inline void bp_test_fail(bp_test_context_t *ctx) {
    bp_test_leave_step(ctx, BP_TEST_SA_FAIL);
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
 * @brief Test logging function.
 *
 * If `ll` is error, this will exit the test after logging.
 *
 * @warning
 *      As the `_` suggests, this should not be called directly. Instead use wrapper macros
 *      which will correctly paste in `line` and `func`!
 */
void _bp_test_logf(bp_test_context_t *ctx, bp_test_log_level_t ll, unsigned int line, const char *func, const char *fmt, ...);

/*
 * NOTE: Log calls for ALL log levels are ALWAYS compiled in.
 *
 * Depending on how the virtual log endpoint is implemented, messaged can be ignored dynamically
 * instead of at compile time.
 */

#define BP_TEST_ERROR(ctx, fmt, ...) \
    _bp_test_logf(ctx, BP_TEST_LL_ERROR, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define BP_TEST_WARN(ctx, fmt, ...) \
    _bp_test_logf(ctx, BP_TEST_LL_WARN, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define BP_TEST_INFO(ctx, fmt, ...) \
    _bp_test_logf(ctx, BP_TEST_LL_INFO, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define BP_TEST_DEBUG(ctx, fmt, ...) \
    _bp_test_logf(ctx, BP_TEST_LL_DEBUG, __LINE__, __func__, fmt, ##__VA_ARGS__)

#endif
