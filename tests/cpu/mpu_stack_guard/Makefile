BOARD ?= nucleo-f767zi

include ../Makefile.cpu_common

USEMODULE += mpu_stack_guard

# boards using highlevel_stdio will crash without printing
FEATURES_BLACKLIST += highlevel_stdio

include $(RIOTBASE)/Makefile.include

ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-infinite-recursion
endif
