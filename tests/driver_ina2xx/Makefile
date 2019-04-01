include ../Makefile.tests_common

USEMODULE += ina220
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_INA2XX_I2C  ?= I2C_DEV\(0\)
TEST_INA2XX_ADDR ?= 0x40

# export parameters
CFLAGS += -DTEST_INA2XX_I2C=$(TEST_INA2XX_I2C)
CFLAGS += -DTEST_INA2XX_ADDR=$(TEST_INA2XX_ADDR)

include $(RIOTBASE)/Makefile.include
