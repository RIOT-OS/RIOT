APPLICATION = nrf24l01p_test_netdev
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += vtimer
USEMODULE += nrf24l01p

# Blacklist boards
BOARD_BLACKLIST := chronos mbed_lpc1768 msb-430 msb-430h native qemu-i386 redbee-econotag telosb \
                   wsn430-v1_3b wsn430-v1_4 z1

# Whitelise boards
BOARD_WHITELIST = stm32f4discovery arduino-due

ifneq (,$(filter stm32f4discovery,$(BOARD)))
	SPI_PORT ?= SPI_0
	CE_PIN   ?= GPIO_8
	CS_PIN   ?= GPIO_7
	IRQ_PIN  ?= GPIO_6
endif

ifneq (,$(filter arduino-due,$(BOARD)))
	SPI_PORT ?= SPI_0
	CE_PIN   ?= GPIO_8
	CS_PIN   ?= GPIO_7
	IRQ_PIN  ?= GPIO_4
endif

include $(RIOTBASE)/Makefile.include

ifneq (,$(SPI_PORT))
export CFLAGS += -DSPI_PORT=$(SPI_PORT)
endif
ifneq (,$(CE_PIN))
export CFLAGS += -DCE_PIN=$(CE_PIN)
endif
ifneq (,$(CS_PIN))
export CFLAGS += -DCS_PIN=$(CS_PIN)
endif
ifneq (,$(IRQ_PIN))
export CFLAGS += -DIRQ_PIN=$(IRQ_PIN)
endif
