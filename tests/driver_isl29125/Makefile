APPLICATION = driver_isl29125

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += isl29125
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_ISL29125_I2C     ?= I2C_DEV\(0\)
TEST_ISL29125_IRQ_PIN ?= GPIO_PIN\(0,0\)

# export parameters
CFLAGS += -DTEST_ISL29125_I2C=$(TEST_ISL29125_I2C)
CFLAGS += -DTEST_ISL29125_IRQ_PIN=$(TEST_ISL29125_IRQ_PIN)

include $(RIOTBASE)/Makefile.include
