include ../Makefile.sys_common

LOW_POWER ?= 0

ifeq (1,$(LOW_POWER))
  USEMODULE += ztimer_msec
endif

USEMODULE += ztimer_usec
USEMODULE += wait_us_spinning

include $(RIOTBASE)/Makefile.include
