APPLICATION = driver_mrf24j40_lowlevel
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += xtimer
USEMODULE += mrf24j40

# set default device parameters in case they are undefined
SPI_PORT ?= SPI_0
CE_PIN   ?= GPIO_PIN\(0,0\)
CS_PIN   ?= GPIO_PIN\(0,1\)
IRQ_PIN  ?= GPIO_PIN\(0,2\)

# export parameters
CFLAGS += -DSPI_PORT=$(SPI_PORT)
CFLAGS += -DCE_PIN=$(CE_PIN)
CFLAGS += -DCS_PIN=$(CS_PIN)
CFLAGS += -DIRQ_PIN=$(IRQ_PIN)

include $(RIOTBASE)/Makefile.include
