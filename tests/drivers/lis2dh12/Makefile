include ../Makefile.tests_common

# as default we use the SPI mode for now, as the I2C mode is not supported, yet
DRIVER ?= lis2dh12_spi

USEMODULE += fmt
USEMODULE += xtimer
USEMODULE += $(DRIVER)

include $(TESTBASE)/../Makefile.include
