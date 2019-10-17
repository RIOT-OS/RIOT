include ../Makefile.tests_common

USEMODULE += nvram_spi
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_NVRAM_SPI_DEV           ?= SPI_DEV\(0\)
TEST_NVRAM_SPI_CS            ?= GPIO_PIN\(0,0\)
TEST_NVRAM_SPI_SIZE          ?= 64
TEST_NVRAM_SPI_ADDRESS_COUNT ?= 1

# export parameters
CFLAGS += -DTEST_NVRAM_SPI_DEV=$(TEST_NVRAM_SPI_DEV)
CFLAGS += -DTEST_NVRAM_SPI_CS=$(TEST_NVRAM_SPI_CS)
CFLAGS += -DTEST_NVRAM_SPI_SIZE=$(TEST_NVRAM_SPI_SIZE)
CFLAGS += -DTEST_NVRAM_SPI_ADDRESS_COUNT=$(TEST_NVRAM_SPI_ADDRESS_COUNT)

include $(RIOTBASE)/Makefile.include
