SRC := seed.c

BASE_MODULE := prng
SUBMODULES := 1

ifneq (,$(filter prng_tinymt32,$(USEMODULE)))
  DIRS += tinymt32
endif

include $(RIOTBASE)/Makefile.base
