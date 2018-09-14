include ../Makefile.tests_common

USEMODULE += xtimer
USEMODULE += srf08

# set default device parameters in case they are undefined
export TEST_SRF08_I2C   ?= I2C_DEV\(0\)
export TEST_SRF08_SPEED ?= I2C_SPEED_NORMAL
export TEST_MODE        ?= SRF08_MODE_CM
export TEST_NUM_ECHOS   ?= 3

# export parameters
CFLAGS += -DTEST_SRF08_I2C=$(TEST_SRF08_I2C)
CFLAGS += -DTEST_SRF08_SPEED=$(TEST_SRF08_SPEED)
CFLAGS += -DTEST_MODE=$(TEST_MODE)
CFLAGS += -DTEST_NUM_ECHOS=$(TEST_NUM_ECHOS)

include $(RIOTBASE)/Makefile.include
