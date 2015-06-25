APPLICATION = driver_lsm6ds3
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += lsm6ds3
USEMODULE += xtimer

ifneq (,$(TEST_LSM6DS3_I2C))
  CFLAGS += -DTEST_LSM6DS3_I2C=$(TEST_LSM6DS3_I2C)
else
  # set random default
  CFLAGS += -DTEST_LSM6DS3_I2C=I2C_1
endif
ifneq (,$(TEST_LSM6DS3_DEV_ADDR))
  CFLAGS += -DTEST_LSM6DS3_DEV_ADDR=$(TEST_LSM6DS3_DEV_ADDR)
else
  # set random default
  CFLAGS += -DTEST_LSM6DS3_DEV_ADDR=106
endif

include $(RIOTBASE)/Makefile.include
