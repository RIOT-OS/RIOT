include ../Makefile.bench_common

# we use thread flags in this benchmark by default, disable on demand
USEMODULE += core_thread_flags
USEMODULE += benchmark

ifeq (llvm,$(TOOLCHAIN))
  # the floating point exception bug is more likely to trigger when build
  # with LLVM, so we just disable LLVM on native as a work around
  TEST_ON_CI_BLACKLIST += native native64
endif

include $(RIOTBASE)/Makefile.include
