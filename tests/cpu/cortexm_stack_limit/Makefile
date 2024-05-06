BOARD ?= nrf5340dk-app

include ../Makefile.cpu_common

FEATURES_REQUIRED += cortexm_stack_limit

include $(RIOTBASE)/Makefile.include

ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-infinite-recursion
endif
