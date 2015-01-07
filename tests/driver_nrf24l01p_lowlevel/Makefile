APPLICATION = driver_nrf24l01p_lowlevel
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += vtimer
USEMODULE += nrf24l01p

FEATURES_REQUIRED = periph_spi

SPI_PORT ?= SPI_0
CE_PIN   ?= GPIO_0
CS_PIN   ?= GPIO_1
IRQ_PIN  ?= GPIO_2

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
