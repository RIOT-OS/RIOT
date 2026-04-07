BOARD ?= infineon-psoc-62

include ../Makefile.drivers_common

ifneq (,$(filter cortex-m4f,$(CPU_CORE)))
USEMODULE += periph_i2c bmi270 ztimer_usec
SRC += main.c
endif

include $(RIOTBASE)/Makefile.include
