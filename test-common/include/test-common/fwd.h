#ifndef BP_TEST_COMMON_FWD_H
#define BP_TEST_COMMON_FWD_H

#include <stdint.h>

/*
 * @file Some forward typedefs and constants.
 */

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
typedef struct bp_test_logger_t bp_test_logger_t;
typedef struct bp_test_step_t bp_test_step_t;
typedef struct bp_test_seq_t bp_test_seq_t;

#endif
