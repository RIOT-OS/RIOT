APPLICATION = driver_nvram_spi
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

USEMODULE += nvram_spi
USEMODULE += xtimer

ifneq (,$(TEST_NVRAM_SPI_DEV))
  CFLAGS += -DTEST_NVRAM_SPI_DEV=$(TEST_NVRAM_SPI_DEV)
else
  # set arbitrary default
  CFLAGS += -DTEST_NVRAM_SPI_DEV=SPI_0
endif
ifneq (,$(TEST_NVRAM_SPI_CS))
  CFLAGS += -DTEST_NVRAM_SPI_CS=$(TEST_NVRAM_SPI_CS)
else
  # set arbitrary default
  CFLAGS += -DTEST_NVRAM_SPI_CS=GPIO\(0,0\)
endif
ifneq (,$(TEST_NVRAM_SPI_SIZE))
  CFLAGS += -DTEST_NVRAM_SPI_SIZE=$(TEST_NVRAM_SPI_SIZE)
else
  # set tiny arbitrary default
  CFLAGS += -DTEST_NVRAM_SPI_SIZE=64
endif
ifneq (,$(TEST_NVRAM_SPI_ADDRESS_COUNT))
  CFLAGS += -DTEST_NVRAM_SPI_ADDRESS_COUNT=$(TEST_NVRAM_SPI_ADDRESS_COUNT)
else
  # set 1 address byte by default, increase if using a larger module for test.
  CFLAGS += -DTEST_NVRAM_SPI_ADDRESS_COUNT=1
endif

include $(RIOTBASE)/Makefile.include
