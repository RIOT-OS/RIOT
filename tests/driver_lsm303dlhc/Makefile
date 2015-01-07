APPLICATION = driver_lsm303dlhc
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

# Define default pin mappings for some boards:
ifneq (,$(filter iot-lab_M3,$(BOARD)))
  export TEST_LSM303DLHC_MAG_ADDR=30
  export TEST_LSM303DLHC_ACC_ADDR=25
  export TEST_LSM303DLHC_I2C ?=I2C_0
endif

USEMODULE += lsm303dlhc
USEMODULE += vtimer

ifneq (,$(TEST_LSM303DLHC_I2C))
  CFLAGS += -DTEST_LSM303DLHC_I2C=$(TEST_LSM303DLHC_I2C)
else
  # set random default
  CFLAGS += -DTEST_LSM303DLHC_I2C=I2C_0
endif
ifneq (,$(TEST_LSM303DLHC_MAG_ADDR))
  CFLAGS += -DTEST_LSM303DLHC_MAG_ADDR=$(TEST_LSM303DLHC_MAG_ADDR)
else
  # set random default
  CFLAGS += -DTEST_LSM303DLHC_MAG_ADDR=30
endif
ifneq (,$(TEST_LSM303DLHC_ACC_ADDR))
  CFLAGS += -DTEST_LSM303DLHC_ACC_ADDR=$(TEST_LSM303DLHC_ACC_ADDR)
else
  # set random default
  CFLAGS += -DTEST_LSM303DLHC_ACC_ADDR=25
endif
ifneq (,$(TEST_LSM303DLHC_ACC_PIN))
  CFLAGS += -DTEST_LSM303DLHC_ACC_PIN=$(TEST_LSM303DLHC_ACC_PIN)
else
  # set random default
  CFLAGS += -DTEST_LSM303DLHC_ACC_PIN=GPIO_0
endif
ifneq (,$(TEST_LSM303DLHC_MAG_PIN))
  CFLAGS += -DTEST_LSM303DLHC_MAG_PIN=$(TEST_LSM303DLHC_MAG_PIN)
else
  # set random default
  CFLAGS += -DTEST_LSM303DLHC_MAG_PIN=GPIO_1
endif

include $(RIOTBASE)/Makefile.include
