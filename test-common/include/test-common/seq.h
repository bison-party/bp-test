#ifndef BP_TEST_COMMON_SEQ_H 
#define BP_TEST_COMMON_SEQ_H

#include <stdarg.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h> // Just used for `size_t`.

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

typedef struct bp_test_step_t {
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
     *
     * __NOTE__ `cookie` could also be accessed via the `step` pointer within
     * `bp_test_context_t`. It is passed to `bp_test_step` for convenience.
     */
    bp_test_step_status_t (*bp_test_step)(bp_test_context_t *ctx, const void *cookie);
} bp_test_step_t;

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

#ifndef BP_TEST_COMMON_CTX_MAP_CAP
/**
 * @brief Static capacity of test context map.
 */
#define BP_TEST_COMMON_CTX_MAP_CAP 32
#endif

typedef struct bp_test_context_impl_t bp_test_context_impl_t;

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
     * @brief Most fundamental logging function for the test step.
     *
     * @param ctx This test context.
     * @param ll The log level.
     * @param line The line number of the log call.
     * @param func The name of the function containing the line call.
     * @param msg The message to log.
     *
     * This function pointer should only exit the test if there is an error actually preforming
     * the log (even in this case, exiting is optional). For example, if `ll` is `BP_T_LL_ERROR`,
     * this endpoint should just log the message just like with any other log level.
     * Wrappers around this endpoint will perform early test exiting logic as needed. 
     * For this reason, this endpoint should __NEVER__ be called directly from a test step!
     * Always use helpers!
     */
    void (* const bp_tc_log)(bp_test_context_t *ctx, bp_test_log_level_t ll, unsigned int line, const char *func, const char *msg);

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

/**
 * @brief Leave a test step early.
 *
 * @param ctx The test context.
 * @param status The status to leave with.
 */
void bp_tc_leave_step(bp_test_context_t *ctx, bp_test_step_status_t status);



#endif
