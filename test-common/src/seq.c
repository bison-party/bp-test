#include "test-common/seq.h"

void _bp_test_logf(bp_test_context_t *ctx, bp_test_log_level_t ll, unsigned int line, const char *func, const char *fmt, ...) {
    char buf[BP_TEST_COMMON_LOG_BUFFER_CAP];

    va_list vargs;
    va_start(vargs, fmt);
    bp_test_vsnprintf(buf, BP_TEST_COMMON_LOG_BUFFER_CAP, fmt, vargs);
    va_end(vargs);

    ctx->bp_tc_log(ctx, ll, line, func, buf);

    if (ll == BP_TEST_LL_ERROR) {
        bp_tc_leave_step(ctx, BP_TEST_SS_FAIL);
    }

    BP_TEST_SS
}
