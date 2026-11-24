#include "test-common/cookie.h"

#include "test-common/deps.h"

static size_t default_cookie_to_str(const void *cookie, size_t n, char *buf) {
    if (n == 0) {
        return 0;
    }

    int vsp_rv = bp_test_snprintf(buf, n, "%p", cookie);

    if (vsp_rv < 0) {
        buf[0] = '\0';
        return 0;
    }

    return (size_t)vsp_rv;
}

static unsigned int default_cookie_hash(const void *cookie) {
    uintptr_t hv = (uintptr_t)cookie;

    // Generic hash function I found on google.
    hv = ((hv >> 16) ^ hv) * 0x45d9f3b;
    hv = ((hv >> 16) ^ hv) * 0x45d9f3b;
    hv = ((hv >> 16) ^ hv);

    return (unsigned int)hv;
}

static bool default_cookie_equals(const void *cookie0, const void *cookie1) {
    // With no sematic knowledge about the underlying type of a cookie, we can only
    // rely on direct pointer comparison for equivelance.
    return cookie0 == cookie1;
}

const bp_test_cookie_impl_t BP_TEST_COMMON_COOKIE_DEFAULT_IMPL = {
    .to_str = default_cookie_to_str,
    .hash = default_cookie_hash,
    .equals = default_cookie_equals
};
