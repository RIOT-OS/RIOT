APPLICATION = driver_isl29020
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += isl29020
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_ISL29020_I2C  ?= I2C_0
TEST_ISL29020_ADDR ?= 68

# export parameters
CFLAGS += -DTEST_ISL29020_I2C=$(TEST_ISL29020_I2C)
CFLAGS += -DTEST_ISL29020_ADDR=$(TEST_ISL29020_ADDR)

include $(RIOTBASE)/Makefile.include
