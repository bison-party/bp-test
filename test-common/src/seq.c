#include "test-common/seq.h"

uint32_t bp_test_step_hash(const bp_test_step_t *test_step) {
    uintptr_t hv = (uintptr_t)(test_step->bp_test_step);

    hv = ((hv >> 16) ^ hv) * 987821;
    hv = ((hv >> 16) ^ hv) * 987869;
    hv = ((hv >> 16) ^ hv);

    uint32_t fhv = (uint32_t)hv;

    if (test_step->cookie) {
        uint32_t cookie_hash;

        if (test_step->cookie_impl->hash) {
            cookie_hash = test_step->cookie_impl->hash(test_step->cookie);
        } else {
            cookie_hash = BP_TEST_COMMON_COOKIE_DEFAULT_IMPL.hash(test_step->cookie);
        }

        fhv = (31 * fhv) + cookie_hash;
    }

    return fhv;
}

bool bp_test_step_equals(const bp_test_step_t *test_step0, const bp_test_step_t *test_step1) {
    if (test_step0 == test_step1) {
        return true;
    }

    if (test_step0->bp_test_step != test_step1->bp_test_step) {
        return false; 
    }

    if (test_step0->cookie == test_step1->cookie) {
        return true;
    }

    // At this point, we have two steps which have the same handler, but cookies with 
    // different addresses.
    //
    // NOTE: It is gauranteed here that `test_step0->cookie` and `test_step1->cookie` are 
    // both Non-NULL and of the same type. 
    // By the conditions listed in `seq.h`, it is impossible for a test step handler to 
    // both accept a NULL cookie and a Non-NULL cookie. The handler must ALWAYS accept 
    // two cookies of the same type.

    if (test_step0->cookie_impl && test_step0->cookie_impl->equals) {
        return test_step0->cookie_impl->equals(test_step0->cookie, test_step1->cookie);
    }

    return BP_TEST_COMMON_COOKIE_DEFAULT_IMPL.equals(test_step0->cookie, test_step1->cookie);
}

void bp_test_context_base_init(bp_test_context_t *ctx, const bp_test_context_impl_t *impl) {
    *(const bp_test_context_impl_t **)&(ctx->impl) = impl;

    for (size_t i = 0; i < BP_TEST_COMMON_CTX_MAP_CAP; i++) {
        ctx->map[i].key = NULL;
    }
}

void bp_test_context_deinit(bp_test_context_t *ctx) {
    if (ctx->impl->deinit) {
        ctx->impl->deinit(ctx);
    }
}



/*
uint32_t bp_test_seq_hash(const bp_test_seq_t *test_seq) {
    uint32_t hv = 23;

    const bp_test_step_t * const *iter = test_seq->steps;

    while (*iter) {
        hv = (hv * 13) + bp_test_step_hash(*(iter++));
    }

    return hv;
}

bool bp_test_seq_equals(const bp_test_seq_t *test_seq0, const bp_test_seq_t *test_seq1) {
    if (test_seq0 == test_seq1) {
        return true;
    }

    // Note that we don't really care about the test sequence (or step) names.

    if (test_seq0->steps == test_seq1->steps) {
        return true; 
    }

    const bp_test_step_t * const *iter0 = test_seq0->steps;
    const bp_test_step_t * const *iter1 = test_seq1->steps;

    while (*iter0 && *iter1) {
        if (!bp_test_step_equals(*(iter0++), *(iter1++))) {
            return false;
        }
    }

    return (!*iter0) && (!*iter1);
}


void bp_test_leave_step(bp_test_context_t *ctx, bp_test_step_action_t action) {
    // TODO
}

void _bp_test_logf(bp_test_context_t *ctx, bp_test_log_level_t ll, unsigned int line, const char *func, const char *fmt, ...) {
    char buf[BP_TEST_COMMON_LOG_BUFFER_CAP];

    va_list vargs;

    va_start(vargs, fmt);
    bp_test_vsnprintf(buf, BP_TEST_COMMON_LOG_BUFFER_CAP, fmt, vargs);
    va_end(vargs);

    ctx->bp_tc_log(ctx, ll, line, func, buf);

    if (ll == BP_TEST_LL_ERROR) {
        bp_test_leave_step(ctx, BP_TEST_SA_FAIL);
    }
}
*/

