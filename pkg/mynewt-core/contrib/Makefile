MODULE = mynewt-core

# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out nrf5x_isr.c cputime.c,$(wildcard *.c))

ifneq (,$(filter nrf5%,$(CPU)))
  SRC += nrf5x_isr.c
else
  SRC += cputime.c
endif

include $(RIOTBASE)/Makefile.base
