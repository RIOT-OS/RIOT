MODULE = kinetis_common_periph

# always build the peripheral drivers that are used internally
SRC = mcg.c wdog.c

# hack to get the hwrng in for now, better ideas?
ifneq (,$(filter periph_hwrng,$(FEATURES_PROVIDED)))
  SRC += hwrng_rnga.c hwrng_rngb.c
endif

include $(RIOTBASE)/Makefile.periph
