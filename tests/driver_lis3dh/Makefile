APPLICATION = driver_lis3dh
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

USEMODULE += lis3dh
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_LIS3DH_SPI  ?= SPI_0
TEST_LIS3DH_CS   ?= GPIO_PIN\(0,0\)
TEST_LIS3DH_INT1 ?= GPIO_PIN\(0,1\)
TEST_LIS3DH_INT2 ?= GPIO_PIN\(0,2\)

# export parameters
CFLAGS += -DTEST_LIS3DH_SPI=$(TEST_LIS3DH_SPI)
CFLAGS += -DTEST_LIS3DH_CS=$(TEST_LIS3DH_CS)
CFLAGS += -DTEST_LIS3DH_INT1=$(TEST_LIS3DH_INT1)
CFLAGS += -DTEST_LIS3DH_INT2=$(TEST_LIS3DH_INT2)

include $(RIOTBASE)/Makefile.include
