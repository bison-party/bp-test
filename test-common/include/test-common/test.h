#ifndef BP_TEST_COMMON_TEST_H 
#define BP_TEST_COMMON_TEST_H

#ifndef BP_TEST_COMMON_SEQ_PHASE_CAP
/**
 * @brief The maximum number of steps in a test sequence phase.
 */
#define BP_TEST_COMMON_SEQ_PHASE_CAP 16
#endif

#ifndef BP_TEST_COMMON_CTX_MAP_CAP
/**
 * @brief Static capacity of test context map.
 */
#define BP_TEST_COMMON_CTX_MAP_CAP 32
#endif


typedef struct bp_test_step_t bp_test_step_t;
typedef struct bp_test_context_t bp_test_context_t;
typedef struct bp_test_context_impl_t bp_test_context_impl_t;

typedef struct bp_test_sequence_t bp_test_sequence_t;

// I feel like a test step should likely always be named imo...
// Yeah, I think you are right here!
//typedef int (*bp_test_step_t)(bp_test_context_t *);

/**
 * @brief An ordered set of steps which makes up one complete test.
 *
 * A test sequence is broken up into 3 phases: `pre`, `body`, and `post`.
 * These phases are somewhat arbitrary, but their intended purposes are aligned with their names.
 * `pre` should do test setup, `body` should perform the significant part of the test, and `post`
 * should do test cleanup.
 *
 * When a test sequence is executing end to end, it can be interrupted in 2 ways.
 *  1. A test step actually does something fatal which causes the testing environment to crash.
 *  2. A test step returns a non-zero error code.
 *
 * __NOTE__ A test *can* fail even when no testing steps return a non-zero error code. 
 * Using the test context, there will be a way to log errors without ending the test.
 * This may be a situation where a there was a soft failure in the test, but cleanup steps
 * should still execute.
 */
struct bp_test_sequence_t {
    // setup steps -> test steps -> cleanup steps.
    // I think these should be named tbh...
    bp_test_step_t pre[BP_TEST_COMMON_SEQ_PHASE_CAP];
    bp_test_step_t body[BP_TEST_COMMON_SEQ_PHASE_CAP];
    bp_test_step_t post[BP_TEST_COMMON_SEQ_PHASE_CAP];
};

struct bp_test_context_impl_t {
    // We'll need logging I think??
    // Also, entrance and exit type shi???
    // Is that necessary though?
    // Maybe we should define some sort of test sequence first??
    // Then come back to this context thing IMO?
};

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
         */
        const char *key;

        /**
         * @brief An arbitrary pointer.
         */
        void *value;
    } map[BP_TEST_COMMON_CTX_MAP_CAP];
    
    const bp_test_context_impl_t * const impl;
};


typedef struct bp_test_impl_t bp_test_impl_t;
typedef struct bp_test_t bp_test_t;


struct bp_test_impl_t {
};

struct bp_test_t {

};


// And how does error handling work???
// What if there were an abstract notion of a test-step?
// Does this return an error if something goes wrong??
// Does this have the ability to exit early or some shi??
// Can we use setjump?
// What actually is a test context also?
// Can we call testing functions from within a function itself??
// I think error return propegation is actually the best here...
// then we can get a stack trace for free...
//
// int test_step(ctx);
// push pre, pop pre
// push post, pop post?
//
// A test is what exactly?
// A test is a...
//
// pre1 -> pre2 -> pre3 -> body -> post3 -> post2 -> post1
// Should there be some sort of error passing here??
// What about fork situation??

// Alright so tests can be nested I guess?

// Alright, well, what should even go in here tbh...
//

#endif
