ifneq (,$(filter mynewt-core,$(USEMODULE)))
  SRC = nimble_port.c
else ifeq (,$(filter nrf51 nrf52 nrf53,$(CPU_FAM)))
  # hal_timer.c does only compile for nRF5x MCUs
  IGNORE := hal_timer.c
  SRC := $(filter-out $(IGNORE),$(wildcard *.c))
endif

include $(RIOTBASE)/Makefile.base
