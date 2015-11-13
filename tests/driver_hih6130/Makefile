APPLICATION = driver_hih6130
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += hih6130
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_HIH6130_I2C  ?= I2C_0
TEST_HIH6130_ADDR ?= 0x27

# export parameters
CFLAGS += -DTEST_HIH6130_I2C=$(TEST_HIH6130_I2C)
CFLAGS += -DTEST_HIH6130_ADDR=$(TEST_HIH6130_ADDR)

include $(RIOTBASE)/Makefile.include
