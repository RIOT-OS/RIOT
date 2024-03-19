ifeq (,$(filter nrf51 nrf52 nrf53,$(CPU_FAM)))
  # nrf5x_isr.c does only compile for nRF5x MCUs
  IGNORE := nrf5x_isr.c
  SRC := $(filter-out $(IGNORE),$(wildcard *.c))
endif

include $(RIOTBASE)/Makefile.base
