APPLICATION = driver_adxl362
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi
FEATURES_REQUIRED = periph_gpio

USEMODULE += adxl362
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_ADXL362_SPI  ?= SPI_1
TEST_ADXL362_CS   ?= 'GPIO_PIN(PORT_E,4)'

# export parameters
CFLAGS += -DTEST_ADXL362_SPI=$(TEST_ADXL362_SPI)
CFLAGS += -DTEST_ADXL362_CS=$(TEST_ADXL362_CS)

include $(RIOTBASE)/Makefile.include
