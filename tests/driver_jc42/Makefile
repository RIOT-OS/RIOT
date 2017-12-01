include ../Makefile.tests_common

USEMODULE += jc42
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_I2C ?= 0
TEST_I2C_ADDR ?= 0x18
TEST_I2C_SPEED ?= I2C_SPEED_NORMAL

# export parameters
CFLAGS += -DTEST_I2C=$(TEST_I2C)
CFLAGS += -DTEST_I2C_ADDR=$(TEST_I2C_ADDR)
CFLAGS += -DTEST_I2C_SPEED=$(TEST_I2C_SPEED)

include $(RIOTBASE)/Makefile.include
