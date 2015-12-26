APPLICATION = driver_mpl3115a2
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += mpl3115a2
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_MPL3115A2_I2C  ?= I2C_0
TEST_MPL3115A2_ADDR ?= 0x60

# export parameters
CFLAGS += -DTEST_MPL3115A2_I2C=$(TEST_MPL3115A2_I2C)
CFLAGS += -DTEST_MPL3115A2_ADDR=$(TEST_MPL3115A2_ADDR)

include $(RIOTBASE)/Makefile.include
