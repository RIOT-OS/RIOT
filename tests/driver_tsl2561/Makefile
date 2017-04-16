APPLICATION = driver_tsl2561
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += tsl2561
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_I2C              ?= I2C_DEV\(0\)
TEST_ADDR             ?= TSL2561_ADDR_FLOAT
TEST_GAIN             ?= TSL2561_GAIN_1X
TEST_INTEGRATION_TIME ?= TSL2561_INTEGRATIONTIME_402MS

# export parameters
CFLAGS += -DTEST_I2C=$(TEST_I2C)
CFLAGS += -DTEST_ADDR=$(TEST_ADDR)
CFLAGS += -DTEST_GAIN=$(TEST_GAIN)
CFLAGS += -DTEST_INTEGRATION_TIME=$(TEST_INTEGRATION_TIME)

include $(RIOTBASE)/Makefile.include
