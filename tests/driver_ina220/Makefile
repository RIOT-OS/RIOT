APPLICATION = driver_ina220
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += ina220
USEMODULE += vtimer

ifneq (,$(TEST_INA220_I2C))
  CFLAGS += -DTEST_INA220_I2C=$(TEST_INA220_I2C)
else
  # set arbitrary default
  CFLAGS += -DTEST_INA220_I2C=I2C_0
endif
ifneq (,$(TEST_INA220_ADDR))
  CFLAGS += -DTEST_INA220_ADDR=$(TEST_INA220_ADDR)
else
  # set arbitrary default
  CFLAGS += -DTEST_INA220_ADDR=0x40
endif

include $(RIOTBASE)/Makefile.include
