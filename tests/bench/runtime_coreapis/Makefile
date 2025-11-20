include ../Makefile.bench_common

# we use thread flags in this benchmark by default, disable on demand
USEMODULE += core_thread_flags
USEMODULE += benchmark

ifeq (llvm,$(TOOLCHAIN))
  # the floating point exception bug is more likely to trigger when build
  # with LLVM, so we just disable LLVM on native as a work around
  TEST_ON_CI_BLACKLIST += native32 native64
endif

include $(RIOTBASE)/Makefile.include

ifeq (1,$(RIOT_CI_BUILD))
  ifneq (,$(filter native%,$(BOARD)))
    # In the CI the background load will mess up benchmarks anyway, so this is
    # more about checking whether the benchmarks actually do run in the CI and
    # not about getting good numbers. So we can reduce the benchmark runs
    # quite a bit.
    CFLAGS += -DBENCH_CLIST_RUNS=50
  endif
endif
