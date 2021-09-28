include ../Makefile.tests_common

DRIVER ?= bme680_i2c

USEMODULE += $(DRIVER)
USEMODULE += ztimer_usec

ifeq ($(ENABLE_FP),1)
  USEMODULE += bme680_fp
endif

include $(RIOTBASE)/Makefile.include
