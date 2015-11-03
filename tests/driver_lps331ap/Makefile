APPLICATION = driver_lps331ap
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += lps331ap
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_LPS331AP_I2C  ?= I2C_0
TEST_LPS331AP_ADDR ?= 92

# export parameters
CFLAGS += -DTEST_LPS331AP_I2C=$(TEST_LPS331AP_I2C)
CFLAGS += -DTEST_LPS331AP_ADDR=$(TEST_LPS331AP_ADDR)

include $(RIOTBASE)/Makefile.include
