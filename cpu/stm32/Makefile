MODULE = cpu

DIRS = $(RIOTCPU)/cortexm_common periph stmclk vectors

ifneq (,$(filter bootloader_stm32,$(USEMODULE)))
  DIRS += bootloader
endif

ifneq (,$(filter lcd_fmc,$(USEMODULE)))
  DIRS += lcd_fmc
endif

include $(RIOTBASE)/Makefile.base
