MODULE = cpu

DIRS = $(RIOTCPU)/cortexm_common periph stmclk vectors

ifneq (,$(filter bootloader_stm32,$(USEMODULE)))
  DIRS += bootloader
endif

include $(RIOTBASE)/Makefile.base
