include ../Makefile.tests_common

# exclude boards with insufficient memory
BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

FEATURES_REQUIRED = periph_spi

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += xtimer
USEMODULE += nrf24l01p

# set default device parameters in case they are undefined
SPI_PORT ?= SPI_DEV\(0\)
CE_PIN   ?= GPIO_PIN\(0,0\)
CS_PIN   ?= GPIO_PIN\(0,1\)
IRQ_PIN  ?= GPIO_PIN\(0,2\)

# export parameters
CFLAGS += -DSPI_PORT=$(SPI_PORT)
CFLAGS += -DCE_PIN=$(CE_PIN)
CFLAGS += -DCS_PIN=$(CS_PIN)
CFLAGS += -DIRQ_PIN=$(IRQ_PIN)

include $(RIOTBASE)/Makefile.include
