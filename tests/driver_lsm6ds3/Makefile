APPLICATION = driver_lsm6ds3
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += lsm6ds3
USEMODULE += xtimer

# The limifrog uses the I2C_1 bus to communicate with the built-in sensors
ifneq (,$(filter limifrog-v1,$(BOARD)))
	TEST_LSM6DS3_I2C=I2C_1
else
	TEST_LSM6DS3_I2C=I2C_0
endif

CFLAGS += -DTEST_LSM6DS3_I2C=$(TEST_LSM6DS3_I2C)

ifneq (,$(TEST_LSM6DS3_DEV_ADDR))
  CFLAGS += -DTEST_LSM6DS3_DEV_ADDR=$(TEST_LSM6DS3_DEV_ADDR)
else
  CFLAGS += -DTEST_LSM6DS3_DEV_ADDR=106
endif

include $(RIOTBASE)/Makefile.include
