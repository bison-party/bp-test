#ifndef BP_TEST_COMMON_LOGGER_H
#define BP_TEST_COMMON_LOGGER_H

#include "test-common/fwd.h"
#include <stddef.h>

typedef uint8_t bp_test_log_level_t;

#define BP_TEST_LOG_LEVEL_ERROR 0
#define BP_TEST_LOG_LEVEL_WARN 1
#define BP_TEST_LOG_LEVEL_INFO 2
#define BP_TEST_LOG_LEVEL_DEBUG 3

typedef struct bp_test_logger_impl_t bp_test_logger_impl_t;

/**
 * @brief A `bp_test_logger_t` is just an interface for passing test output from the test executor
 * to some ingester.
 */
struct bp_test_logger_t {
    const bp_test_logger_impl_t * const impl;
};

struct bp_test_logger_impl_t {
    /**
     * @brief OPTIONAL virtual deinit.
     *
     * @param logger    The logger to deinitialize.
     */
    void (*deinit)(bp_test_logger_t *logger);

    /**
     * @brief Callback which is executed right before a test sequence is entered.
     *
     * This function is entirely OPTIONAL.
     *
     * @param lgr   The test logger. Alway Non-NULL.
     * @param seq   The sequence about to be entered. Always Non-NULL.
     */
    void (*on_enter_seq)(bp_test_logger_t *lgr, const bp_test_seq_t *seq);

    /**
     * @brief Callback which is executed right before a test step is entered.
     *
     * This function is entirely OPTIONAL.
     *
     * @param lgr           The test logger. Alway Non-NULL.
     * @param step          The step about to be entered. Always Non-NULL.
     * @param step_index    The index of the step within the sequence. This is really just
     *                      for convenience.
     */
    void (*on_enter_step)(bp_test_logger_t *lgr, const bp_test_step_t *step, size_t step_index);

    /**
     * @brief Logging endpoint which is useable from within an executing test step.
     *
     * This function is entirely OPTIONAL.
     *
     * @param lgr   This logger. Always Non-NULL.
     * @param ll    The log level.
     * @param line  The line number of the log call.
     * @param func  The name of the function containing the line call. Always Non-NULL.
     * @param msg   The message to log. Always Non-NULL.
     */
    void (*log_from_test_step)(bp_test_logger_t *lgr, bp_test_log_level_t ll, unsigned int line,
            const char *func, const char *msg);

    /**
     * @brief Callback which is executed right after a step is exited.
     *
     * This function is entirely OPTIONAL.
     *
     * @param lgr           The test logger. Alway Non-NULL.
     * @param step          The step which just exited. Always Non-NULL.
     * @param step_index    The index of the step within the sequence. This is really just
     *                      for convenience.
     * @param result        The resulting action of the test step.
     */
    void (*on_exit_step)(bp_test_logger_t *lgr, const bp_test_step_t *step, size_t step_index,
            bp_test_step_action_t result);

    /**
     * @brief Callback which is executed right after a test sequence is exited.
     *
     * This function is entirely OPTIONAL.
     *
     * @param lgr           The test logger. Alway Non-NULL.
     * @param seq           The sequence which just exited.
     * @param step          The last step which exited during the test sequence.
     * @param step_index    The index of the last step which exited during the test sequence.
     * @param result        The result of the last step which exited during the test sequence.
     *
     * __NOTE__ `step`, `step_index`, and `result` are really just for convenience here.
     * They just help with when a sequence exits early due to either an early pass or a failure.
     */
    void (*on_exit_seq)(bp_test_logger_t *lgr, const bp_test_seq_t *seq, const bp_test_step_t *step,
            size_t step_index, bp_test_step_action_t result);
};

#endif
