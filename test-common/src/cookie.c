#include "test-common/cookie.h"

#include "test-common/deps.h"

static size_t default_cookie_to_str(const void *cookie, size_t n, char *buf) {
    
}

static uint32_t default_cookie_hash(const void *cookie) {

}

static bool default_cookie_equals(const void *cookie0, const void *cookie1) {

}

const bp_test_cookie_impl_t BP_TEST_COMMON_COOKIE_DEFAULT_IMPL;
