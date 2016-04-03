APPLICATION = driver_si70xx
include ../Makefile.tests_common

FEATURES_REQUIRED += periph_gpio
FEATURES_REQUIRED += periph_i2c

USEMODULE += si70xx
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_I2C ?= 0
TEST_I2C_ADDR ?= 0x80
TEST_PIN_EN ?= GPIO_PIN\(0,0\)

# export parameters
CFLAGS += -DTEST_I2C=$(TEST_I2C)
CFLAGS += -DTEST_I2C_ADDR=$(TEST_I2C_ADDR)
CFLAGS += -DTEST_PIN_EN=$(TEST_PIN_EN)

include $(RIOTBASE)/Makefile.include
