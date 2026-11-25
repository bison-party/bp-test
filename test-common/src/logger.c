
#include "test-common/logger.h"

void bp_test_logger_init_base(bp_test_logger_t *logger, const bp_test_logger_impl_t *impl) {
    *(const bp_test_logger_impl_t **)&(logger->impl) = impl; 
}

void bp_test_logger_deinit(bp_test_logger_t *logger) {
    if (logger->impl->deinit) {
        logger->impl->deinit(logger);
    }
}
