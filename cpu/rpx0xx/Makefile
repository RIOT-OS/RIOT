MODULE = cpu

MODULE_DEFS_NEEDED += \
  periph_adc \
  #

DIRS = $(RIOTCPU)/cortexm_common periph

ifneq (,$(filter pio_i2c,$(USEMODULE)))
  DIRS += pio/i2c
endif

include $(RIOTBASE)/Makefile.base
