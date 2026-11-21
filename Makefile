
GIT_TOP := $(shell git rev-parse --show-toplevel)
BP_BUILD := $(GIT_TOP)/deps/bp-build

MODS := $(abspath ./test-common)

include $(BP_BUILD)/colors.mk
include $(BP_BUILD)/make-c/project.mk
