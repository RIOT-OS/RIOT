APPLICATION = driver_ina220
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += ina220
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_INA220_I2C  ?= I2C_0
TEST_INA220_ADDR ?= 0x40

# export parameters
CFLAGS += -DTEST_INA220_I2C=$(TEST_INA220_I2C)
CFLAGS += -DTEST_INA220_ADDR=$(TEST_INA220_ADDR)

include $(RIOTBASE)/Makefile.include
