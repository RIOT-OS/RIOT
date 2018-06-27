# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += isl29125
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
