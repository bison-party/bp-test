#ifndef BP_TEST_COMMON_TEST_H 
#define BP_TEST_COMMON_TEST_H

#ifndef BP_TEST_COMMON_CTX_MAP_CAP
/**
 * @brief static capacity of test context map.
 */
#define BP_TEST_COMMON_CTX_MAP_CAP 32
#endif

typedef struct bp_test_context_t {

} bp_test_context_t;

typedef int (*bp_test_step_t)(bp_test_context_t *);

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
