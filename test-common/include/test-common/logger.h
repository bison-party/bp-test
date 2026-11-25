#ifndef BP_TEST_COMMON_LOGGER_H
#define BP_TEST_COMMON_LOGGER_H

#include <stddef.h>
#include <stdint.h>

typedef uint8_t bp_test_log_level_t;

#define BP_TEST_LOG_LEVEL_ERROR 0
#define BP_TEST_LOG_LEVEL_WARN 1
#define BP_TEST_LOG_LEVEL_INFO 2
#define BP_TEST_LOG_LEVEL_DEBUG 3

typedef struct bp_test_logger_t bp_test_logger_t;
typedef struct bp_test_step_t bp_test_step_t;
typedef struct bp_test_seq_t bp_test_seq_t;

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
     * OPTIONAL
     *
     * @see `bp_test_logger_deinit`.
     */
    void (*deinit)(bp_test_logger_t *logger);

    /**
     * OPTIONAL
     *
     * @see `bp_test_logger_on_enter_seq`.
     */
    void (*on_enter_seq)(bp_test_logger_t *lgr, const bp_test_seq_t *seq);

    /**
     * OPTIONAL
     *
     * @see `bp_test_logger_on_enter_step`.
     */
    void (*on_enter_step)(bp_test_logger_t *lgr, const bp_test_step_t *step, size_t step_index);

    /**
     * OPTIONAL
     *
     * @see `bp_test_logger_log_from_test_step`.
     */
    void (*log_from_test_step)(bp_test_logger_t *lgr, bp_test_log_level_t ll, unsigned int line,
            const char *func, const char *msg);

    /**
     * OPTIONAL
     *
     * @see `bp_test_logger_on_exit_step`.
     */
    void (*on_exit_step)(bp_test_logger_t *lgr, const bp_test_step_t *step, size_t step_index,
            bp_test_step_action_t result);

    /**
     * OPTIONAL
     *
     * @see `bp_test_logger_on_exit_seq`.
     */
    void (*on_exit_seq)(bp_test_logger_t *lgr, const bp_test_seq_t *seq, const bp_test_step_t *step,
            size_t step_index, bp_test_step_action_t result);
};

/**
 * @brief Initialize the bp test logger base class.
 *
 * @param logger    The logger to initialize. Always Non-NULL.
 * @param impl      The implementation. Always Non-NULL.
 */
void bp_test_logger_init_base(bp_test_logger_t *lgr, const bp_test_logger_impl_t *impl);

/**
 * @brief Virtual deinitializer.
 *
 * @param logger    The logger to deinitialize.
 */
void bp_test_logger_deinit(bp_test_logger_t *lgr);

/**
 * @brief Callback which is executed right before a test sequence is entered.
 *
 * @param lgr   The test logger. Alway Non-NULL.
 * @param seq   The sequence about to be entered. Always Non-NULL.
 */
static inline void bp_test_logger_on_enter_seq(bp_test_logger_t *lgr, const bp_test_seq_t *seq) {
    if (lgr->impl->on_enter_seq) {
        lgr->impl->on_enter_seq(lgr, seq);
    }
}

/**
 * @brief Callback which is executed right before a test step is entered.
 *
 * @param lgr           The test logger. Alway Non-NULL.
 * @param step          The step about to be entered. Always Non-NULL.
 * @param step_index    The index of the step within the sequence. This is really just
 *                      for convenience.
 */
static inline void bp_test_logger_on_enter_step(bp_test_logger_t *lgr, const bp_test_step_t *step, size_t step_index) {
    if (lgr->impl->on_enter_step) {
        lgr->impl->on_enter_step(lgr, step, step_index); 
    }
}

/**
 * @brief Logging endpoint which is useable from within an executing test step.
 *
 * @param lgr   This logger. Always Non-NULL.
 * @param ll    The log level.
 * @param line  The line number of the log call.
 * @param func  The name of the function containing the line call. Always Non-NULL.
 * @param msg   The message to log. Always Non-NULL.
 */
static inline void bp_test_logger_log_from_test_step(bp_test_logger_t *lgr, bp_test_log_level_t ll, unsigned int line,
        const char *func, const char *msg) {
    if (lgr->impl->log_from_test_step) {
        lgr->impl->log_from_test_step(lgr, ll, line, func, msg);
    }
}

/**
 * @brief Callback which is executed right after a step is exited.
 *
 * @param lgr           The test logger. Alway Non-NULL.
 * @param step          The step which just exited. Always Non-NULL.
 * @param step_index    The index of the step within the sequence. This is really just
 *                      for convenience.
 * @param result        The resulting action of the test step.
 */
static inline void bp_test_logger_on_exit_step(bp_test_logger_t *lgr, const bp_test_step_t *step, size_t step_index,
        bp_test_step_action_t result) {
    if (lgr->impl->on_exit_step) {
        lgr->impl->on_exit_step(lgr, step, step_index, result);
    }
}

/**
 * @brief Callback which is executed right after a test sequence is exited.
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
static inline void bp_test_logger_on_exit_seq(bp_test_logger_t *lgr, const bp_test_seq_t *seq, const bp_test_step_t *step,
        size_t step_index, bp_test_step_action_t result) {
    if (lgr->impl->on_exit_seq) {
        lgr->impl->on_exit_seq(lgr, seq, step, step_index, result);
    }
}

#endif
