include ../Makefile.tests_common

BOARD ?= native

USEMODULE += soft_spi

# set Soft SPI bus and pins to default values
TEST_SOFT_SPI_DEV ?= SOFT_SPI_DEV\(0\)
TEST_CS_PIN ?= GPIO_PIN\(0,2\)

# export SPI and pins
CFLAGS += -DTEST_SOFT_SPI_DEV=$(TEST_SOFT_SPI_DEV)
CFLAGS += -DTEST_CS_PIN=$(TEST_CS_PIN)

include $(RIOTBASE)/Makefile.include
