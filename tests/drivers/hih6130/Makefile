include ../Makefile.drivers_common

USEMODULE += fmt
USEMODULE += hih6130
USEMODULE += ztimer_msec

# set default device parameters in case they are undefined
TEST_HIH6130_I2C  ?= I2C_DEV\(0\)
TEST_HIH6130_ADDR ?= 0x27

# export parameters
CFLAGS += -DTEST_HIH6130_I2C=$(TEST_HIH6130_I2C)
CFLAGS += -DTEST_HIH6130_ADDR=$(TEST_HIH6130_ADDR)

include $(RIOTBASE)/Makefile.include
