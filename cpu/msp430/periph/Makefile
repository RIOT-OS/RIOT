MODULE = periph

# All supported MSP430 families use special serial IP block that implements
# various serial bus interfaces using the same hardware block. For the
# MSP430 x1xx family, the block is called USART. For the MSP430 F2xx/G2xx
# family it is the (incompatible) USCI.
ifeq (msp430_x1xx,$(CPU_FAM))
  SERIAL_IP_BLOCK := usart
endif
ifeq (msp430_f2xx_g2xx,$(CPU_FAM))
  SERIAL_IP_BLOCK := usci
endif

SRC += $(SERIAL_IP_BLOCK).c

# select family specific peripheral drivers.
ifneq (,$(filter periph_uart,$(USEMODULE)))
  SRC += uart_$(SERIAL_IP_BLOCK).c
endif

ifneq (,$(filter periph_spi,$(USEMODULE)))
  SRC += spi_$(SERIAL_IP_BLOCK).c
endif

include $(RIOTMAKE)/periph.mk
