
MODULE_DEFS_NEEDED += \
  periph_cpuid \
  periph_hwrng \
  periph_init_hwrng \
  prng_sha1prng \
  prng_sha256prng \
  puf_sram \
  #
SRC := random.c

BASE_MODULE := prng
SUBMODULES := 1

ifneq (,$(filter prng_fortuna,$(USEMODULE)))
  DIRS += fortuna
endif

ifneq (,$(filter prng_tinymt32,$(USEMODULE)))
  DIRS += tinymt32
endif

include $(RIOTBASE)/Makefile.base
