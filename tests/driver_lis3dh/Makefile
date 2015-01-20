APPLICATION = driver_lis3dh
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

USEMODULE += lis3dh
USEMODULE += vtimer

ifneq (,$(TEST_LIS3DH_SPI))
  CFLAGS += -DTEST_LIS3DH_SPI=$(TEST_LIS3DH_SPI)
else
  # set arbitrary default
  CFLAGS += -DTEST_LIS3DH_SPI=SPI_0
endif
ifneq (,$(TEST_LIS3DH_CS))
  CFLAGS += -DTEST_LIS3DH_CS=$(TEST_LIS3DH_CS)
else
  # set arbitrary default
  CFLAGS += -DTEST_LIS3DH_CS=GPIO_0
endif
ifneq (,$(TEST_LIS3DH_INT1))
  CFLAGS += -DTEST_LIS3DH_INT1=$(TEST_LIS3DH_INT1)
else
  # set arbitrary default
  CFLAGS += -DTEST_LIS3DH_INT1=GPIO_1
endif
ifneq (,$(TEST_LIS3DH_INT2))
  CFLAGS += -DTEST_LIS3DH_INT2=$(TEST_LIS3DH_INT2)
else
  # set arbitrary default
  CFLAGS += -DTEST_LIS3DH_INT2=GPIO_2
endif

include $(RIOTBASE)/Makefile.include
