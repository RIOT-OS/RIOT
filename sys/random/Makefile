# module name is used below, thus set explicitly
MODULE = random

SRC := random.c init.c

BASE_MODULE := prng
SUBMODULES := 1

ifneq (,$(filter prng_fortuna,$(USEMODULE)))
  DIRS += fortuna
endif

ifneq (,$(filter prng_tinymt32,$(USEMODULE)))
  DIRS += tinymt32
endif

ifeq (,$(RIOT_CI_BUILD))
  # random seed value newly generated on each build
  RANDOM_SEED := $(shell $(RIOTTOOLS)/randhex/randhex.py 32)
else
  # hardcoded canary value to not break CI test cache
  RANDOM_SEED := 0x1
endif

# This has to be kept in sync with sys/random/init.c
ifneq (,$(filter puf_sram periph_hwrng periph_cpuid,$(USEMODULE)))
  HAS_RNG = 1
endif

# If no run-time random number generation facility is available for
# generating an initial seed, generate a random value at compile-time.
# Also mark the file using it as PHONY to use a new seed on each build.
ifndef HAS_RNG
  $(warning Attention! The random module is using a compile-time seed. \
    The same seed will be used for the same firmware image.)

  CFLAGS += -DRANDOM_SEED_DEFAULT=$(RANDOM_SEED)U
  .PHONY: $(BINDIR)/$(MODULE)/init.o
endif

include $(RIOTBASE)/Makefile.base
