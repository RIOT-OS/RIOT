BOARD ?= hifive1b

include ../Makefile.cpu_common

USEMODULE += pmp_stack_guard

include $(RIOTBASE)/Makefile.include

ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-infinite-recursion
endif
