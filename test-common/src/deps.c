#include "test-common/deps.h"

int bp_test_snprintf(char *buf, size_t n, const char *fmt, ...) {
    va_list vargs;

    int rv;

    va_start(vargs, fmt);
    rv = bp_test_vsnprintf(buf, n, fmt, vargs);
    va_end(vargs);

    return rv;
}
