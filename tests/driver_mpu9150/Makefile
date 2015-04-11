APPLICATION = driver_mpu9150
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

# Define default mappings for some boards:
ifneq (,$(filter msbiot,$(BOARD)))
  export TEST_I2C ?= MPU9150_I2C
  export TEST_HW_ADDR ?= MPU9150_HW_ADDR
  export TEST_COMP_ADDR ?= MPU9150_COMP_ADDR
endif

USEMODULE += mpu9150
USEMODULE += vtimer

ifneq (,$(TEST_I2C))
  CFLAGS += -DTEST_I2C=$(TEST_I2C)
else
  # set random default
  CFLAGS += -DTEST_I2C=I2C_0
endif
ifneq (,$(TEST_HW_ADDR))
  CFLAGS += -DTEST_HW_ADDR=$(TEST_HW_ADDR)
else
  # set random default
  CFLAGS += -DTEST_HW_ADDR=MPU9150_HW_ADDR_HEX_68
endif
ifneq (,$(TEST_COMP_ADDR))
  CFLAGS += -DTEST_COMP_ADDR=$(TEST_COMP_ADDR)
else
  # set random default
  CFLAGS += -DTEST_COMP_ADDR=MPU9150_COMP_ADDR_HEX_0C
endif

include $(RIOTBASE)/Makefile.include
