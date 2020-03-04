BOARD ?= samr21-xpro
include ../Makefile.tests_common

USEMODULE += mpu_stack_guard

include $(RIOTBASE)/Makefile.include

ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-infinite-recursion
endif
