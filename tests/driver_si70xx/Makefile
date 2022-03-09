include ../Makefile.tests_common

# This test should also work with si7006, si7013, si7020 and si705x variants.
SI70XX_VARIANT ?= si7021

USEMODULE += $(SI70XX_VARIANT)

USEMODULE += ztimer
USEMODULE += ztimer_msec

include $(RIOTBASE)/Makefile.include
