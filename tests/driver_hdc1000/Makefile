APPLICATION = driver_hdc1000
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += hdc1000
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_HDC1000_I2C  ?= I2C_0
TEST_HDC1000_ADDR ?= 0x43

# export parameters
CFLAGS += -DTEST_HDC1000_I2C=$(TEST_HDC1000_I2C)
CFLAGS += -DTEST_HDC1000_ADDR=$(TEST_HDC1000_ADDR)

include $(RIOTBASE)/Makefile.include
