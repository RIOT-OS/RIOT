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
