
ifeq ($(MOD_NAME),)
$(error MOD_NAME is required)
endif

ifeq ($(_HDRS),)
$(error _HDRS is required)
endif

ifeq ($(_SRCS),)
$(error _SRCS is required)
endif

ifeq ($(_TEST_SRCS),)
$(error _TEST_SRCS is required)
endif



