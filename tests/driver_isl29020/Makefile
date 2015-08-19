APPLICATION = driver_isl29020
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

# Define default pin mappings for some boards:
ifneq (,$(filter iotlab_m3,$(BOARD)))
  export TEST_ISL29020_ADDR = 68
  export TEST_ISL29020_I2C ?= I2C_0
endif

USEMODULE += isl29020
USEMODULE += vtimer

ifneq (,$(TEST_ISL29020_I2C))
  CFLAGS += -DTEST_ISL29020_I2C=$(TEST_ISL29020_I2C)
else
  # set random default
  CFLAGS += -DTEST_ISL29020_I2C=I2C_0
endif
ifneq (,$(TEST_ISL29020_ADDR))
  CFLAGS += -DTEST_ISL29020_ADDR=$(TEST_ISL29020_ADDR)
else
  # set random default
  CFLAGS += -DTEST_ISL29020_ADDR=68
endif

include $(RIOTBASE)/Makefile.include
