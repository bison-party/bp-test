######################################## CONFIG_HDR Inputs #########################################

# None

######################################## CONFIG_MK Inputs ##########################################

# None

####################################################################################################

ifdef CONFIG_MK
include $(CONFIG_MK)
endif

GIT_TOP := $(shell git rev-parse --show-toplevel)
BP_BUILD := $(GIT_TOP)/deps/bp-build

C_SRC_NAMES := deps cookie seq
C_TEST_SRC_NAMES := 

CFLAGS := -Wall -Wextra -Wpedantic -Werror

include $(BP_BUILD)/colors.mk
include $(BP_BUILD)/make-c/module.mk
