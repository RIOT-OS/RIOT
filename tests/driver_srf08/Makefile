APPLICATION = driver_srf08
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += vtimer
USEMODULE += srf08

# Define default settings
export TEST_SRF08_I2C ?= I2C_0
export TEST_SRF08_SPEED ?= I2C_SPEED_NORMAL
export TEST_MODE ?= SRF08_MODE_CM
export TEST_NUM_ECHOS?= 3

include $(RIOTBASE)/Makefile.include

ifneq (,$(TEST_SRF08_I2C))
export CFLAGS += -DTEST_SRF08_I2C=$(TEST_SRF08_I2C)
endif
ifneq (,$(TEST_SRF08_SPEED))
export CFLAGS += -DTEST_SRF08_SPEED=$(TEST_SRF08_SPEED)
endif
ifneq (,$(TEST_MODE))
export CFLAGS += -DTEST_MODE=$(TEST_MODE)
endif
ifneq (,$(TEST_NUM_ECHOS))
export CFLAGS += -DTEST_NUM_ECHOS=$(TEST_NUM_ECHOS)
endif
