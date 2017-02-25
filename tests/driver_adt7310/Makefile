APPLICATION = driver_adt7310
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-mega2560 waspmote-pro

FEATURES_REQUIRED = periph_spi periph_gpio

USEMODULE += adt7310
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_ADT7310_SPI ?= SPI_DEV\(0\)
TEST_ADT7310_CS  ?= GPIO_PIN\(0,0\)

# export parameters
CFLAGS += -DTEST_ADT7310_SPI=$(TEST_ADT7310_SPI)
CFLAGS += -DTEST_ADT7310_CS=$(TEST_ADT7310_CS)

include $(RIOTBASE)/Makefile.include
