include ../Makefile.tests_common

USEMODULE += xtimer
USEMODULE += srf02
USEMODULE += shell

# set default device parameters in case they are undefined
TEST_SRF02_I2C ?= I2C_DEV\(0\)
TEST_MODE      ?= SRF02_MODE_REAL_CM

# export parameters
CFLAGS += -DTEST_SRF02_I2C=$(TEST_SRF02_I2C)
CFLAGS += -DTEST_MODE=$(TEST_MODE)

include $(RIOTBASE)/Makefile.include
