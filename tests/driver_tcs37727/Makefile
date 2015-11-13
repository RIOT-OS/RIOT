APPLICATION = driver_tcs37727
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += tcs37727
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_TCS37727_I2C  ?= I2C_0
TEST_TCS37727_ADDR ?= 0x29

# export parameters
CFLAGS += -DTEST_TCS37727_I2C=$(TEST_TCS37727_I2C)
CFLAGS += -DTEST_TCS37727_ADDR=$(TEST_TCS37727_ADDR)

include $(RIOTBASE)/Makefile.include
