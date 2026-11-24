#include "test-common/seq.h"

unsigned int bp_test_step_hash(const bp_test_step_t *test_step) {
    uintptr_t hv = (uintptr_t)(test_step->bp_test_step);

    hv = ((hv >> 16) ^ hv) * 987821;
    hv = ((hv >> 16) ^ hv) * 987869;
    hv = ((hv >> 16) ^ hv);

    unsigned int fhv = (unsigned int)hv;

    if (test_step->cookie) {
        unsigned int cookie_hash;

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
        ctx->map[i].value = NULL;
    }
}

void bp_test_context_deinit(bp_test_context_t *ctx) {
    if (ctx->impl->deinit) {
        ctx->impl->deinit(ctx);
    }
}

/**
 * @brief Hash a string.
 *
 * @param key   The string to hash. Always Non-NULL.
 *
 * @return The hash value.
 */
static unsigned int bp_test_context_string_hash(const char *key) {
    unsigned int hash = 17;

    for (const char *iter = key; *iter != '\0'; iter++) {
        hash ^= (uint8_t)*iter; 
        hash *= 13;
    }

    return hash;
}

/**
 * @brief Compare two strings for equality.
 *
 * @param key0  One key. Always Non-NULL.
 * @param key1  Another key. Always Non-NULL.
 *
 * @return `true` if and only if the characters of `key0` and `key1` are equal.
 */
static bool bp_test_context_string_equals(const char *key0, const char *key1) {
    if (key0 == key1) {
        return true;
    }

    const char *iter0 = key0;
    const char *iter1 = key1;

    while (*iter0 == *iter1) {
        if (*iter0 == '\0') {
            return true;
        }

        iter0++;
        iter1++;
    }

    return false;
}

/*
 * The test context map will follow a simple linear probing scheme.
 * There will be no tombstones, removing will backwards shift.
 */

/**
 * @brief Lookup a key in the context map.
 *
 * @param ctx   The context. Always Non-NULL.
 * @param key   The key to lookup.
 *
 * @return `i` such that `ctx->map[i] streqs key`. If `key` never appears in the map, then
 * the index of the first empty slot in `key`'s linear chain is returned. If the map is full or 
 * `key` is `NULL`, `BP_TEST_COMMON_CTX_MAP_CAP` is returned.
 */
static size_t bp_test_context_find(bp_test_context_t *ctx, const char *key) {
    if (!key) {
        return BP_TEST_COMMON_CTX_MAP_CAP;
    }

    unsigned int key_hash = bp_test_context_string_hash(key);
    size_t key_ind = key_hash % BP_TEST_COMMON_CTX_MAP_CAP;

    for (size_t i = key_ind; i < BP_TEST_COMMON_CTX_MAP_CAP; i++) {
        const char *slot_key = ctx->map[i].key;

        if (!slot_key || bp_test_context_string_equals(key, slot_key)) {
            return i;
        }
    }

    for (size_t i = 0; i < key_ind; i++) {
        const char *slot_key = ctx->map[i].key;

        if (!slot_key || bp_test_context_string_equals(key, slot_key)) {
            return i;
        }
    }

    return BP_TEST_COMMON_CTX_MAP_CAP;
}

bool bp_test_context_put(bp_test_context_t *ctx, const char *key, void *value) {
    if (!key || !value) {
        return false;
    }

    size_t key_slot = bp_test_context_find(ctx, key);

    if (key_slot == BP_TEST_COMMON_CTX_MAP_CAP) {
        return false; // Map is full.
    }

    ctx->map[key_slot].key = key;
    ctx->map[key_slot].value = value;

    return true;
}

bool bp_test_context_remove(bp_test_context_t *ctx, const char *key) {
    size_t key_slot = bp_test_context_find(ctx, key);

    if (key_slot == BP_TEST_COMMON_CTX_MAP_CAP) {
        return false; // `key` is NULL or (Map is full and `key` never appears)
    }

    if (ctx->map[key_slot].key == NULL) {
        return false; // An empty space is hit before `key` in its corresponding
                      // linear chain.
    }

    // `key` was found at `key_slot`, we must shift all following kvps one to the left (cyclically)
    // until we hit a space or `key_ind`.

    unsigned int key_hash = bp_test_context_string_hash(key);
    size_t key_ind = key_hash % BP_TEST_COMMON_CTX_MAP_CAP;

    size_t iter = key_slot;

    while (true) {
        size_t next = (iter + 1) % BP_TEST_COMMON_CTX_MAP_CAP; 

        if (next == key_ind || !(ctx->map[next].key)) {
            ctx->map[iter].key = NULL;
            ctx->map[iter].value = NULL;

            return true;
        } else {
            ctx->map[iter].key = ctx->map[next].key;
            ctx->map[iter].value = ctx->map[next].value;
        }
    }
}

void *bp_test_context_get(bp_test_context_t *ctx, const char *key) {
    size_t key_slot = bp_test_context_find(ctx, key);

    if (key_slot == BP_TEST_COMMON_CTX_MAP_CAP) {
        return NULL; // `key` is NULL or (Map is full and `key` never appears)
    }

    // If `key_slot` is occupied `value` is gauranteed to be Non-NULL.
    // If `key_slot` is empty `value` is gauranteed to be NULL.
    // Thus, we can return value as is here with no checks.

    return ctx->map[key_slot].value;
}

void bp_test_leave_step(bp_test_context_t *ctx, bp_test_step_action_t action) {
    longjmp(ctx->jb, action);
}

void bp_test_context_logf(bp_test_context_t *ctx, bp_test_log_level_t ll, unsigned int line, const char *func, const char *fmt, ...) {
    if (ctx->impl->log) {
        char buf[BP_TEST_COMMON_LOGF_BUFFER_CAP];
        va_list vargs;

        va_start(vargs, fmt);
        bp_test_vsnprintf(buf, BP_TEST_COMMON_LOGF_BUFFER_CAP, fmt, vargs);
        va_end(vargs);

        ctx->impl->log(ctx, ll, line, func, buf);
    }

    if (ll == BP_TEST_LOG_LEVEL_ERROR) {
        bp_test_context_leave(ctx, BP_TEST_STEP_ACTION_FAIL);
    }
}


/*
unsigned int bp_test_seq_hash(const bp_test_seq_t *test_seq) {
    unsigned int hv = 23;

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


*/

