MODULE = board
DIRS = $(RIOTBOARD)/common/nucleo

include $(RIOTBASE)/Makefile.base

ifneq (,$(filter stm32_eth,$(USEMODULE)))
  $(info Remapping MOSI of SPI_DEV(0) from PA7 to PB5 to solve pin conflict.\
         (PA7 is also connected to the RMII_DV of the Ethernet Phy.))
endif
