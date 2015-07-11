APPLICATION = driver_adt7310
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

USEMODULE += adt7310
USEMODULE += vtimer

ifneq (,$(TEST_ADT7310_SPI))
  CFLAGS += -DTEST_ADT7310_SPI=$(TEST_ADT7310_SPI)
else
  # set arbitrary default
  CFLAGS += -DTEST_ADT7310_SPI=SPI_0
endif
ifneq (,$(TEST_ADT7310_CS))
  CFLAGS += -DTEST_ADT7310_CS=$(TEST_ADT7310_CS)
else
  # set arbitrary default
  CFLAGS += -DTEST_ADT7310_CS=GPIO\(0,0\)
endif

include $(RIOTBASE)/Makefile.include
