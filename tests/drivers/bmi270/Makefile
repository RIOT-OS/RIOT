BOARD ?= infineon-cy8ckit-062s2-ai

include ../Makefile.drivers_common

ifneq (,$(filter cortex-m4f,$(CPU_CORE)))
USEMODULE += periph_i2c bmi270 ztimer_usec
SRC += main.c
endif

include $(RIOTBASE)/Makefile.include
