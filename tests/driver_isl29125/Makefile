APPLICATION = driver_isl29125
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += isl29125
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_ISL29125_I2C     ?= I2C_0
TEST_ISL29125_IRQ_PIN ?= GPIO_PIN\(0,0\)

# export parameters
CFLAGS += -DTEST_ISL29125_I2C=$(TEST_ISL29125_I2C)
CFLAGS += -DTEST_ISL29125_IRQ_PIN=$(TEST_ISL29125_IRQ_PIN)

include $(RIOTBASE)/Makefile.include
