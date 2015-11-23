ifeq (,$(filter prng_%,$(USEMODULE)))
    USEMODULE += prng_mersenne
endif
ifneq (,$(filter prng_mersenne,$(USEMODULE)))
    SRC += mersenne.c
endif
ifneq (,$(filter prng_minstd,$(USEMODULE)))
    SRC += minstd.c
endif
ifneq (,$(filter prng_musl_lcg,$(USEMODULE)))
    SRC += musl_lcg.c
endif

include $(RIOTBASE)/Makefile.base
