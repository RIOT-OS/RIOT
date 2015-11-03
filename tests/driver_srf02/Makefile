APPLICATION = driver_srf02
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += xtimer
USEMODULE += srf02

# set default device parameters in case they are undefined
TEST_SRF02_I2C ?= I2C_0
TEST_MODE      ?= SRF02_MODE_REAL_CM

# export parameters
CFLAGS += -DTEST_SRF02_I2C=$(TEST_SRF02_I2C)
CFLAGS += -DTEST_MODE=$(TEST_MODE)

include $(RIOTBASE)/Makefile.include
