APPLICATION = driver_srf02
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += vtimer
USEMODULE += srf02

# Define default settings
export TEST_SRF02_I2C ?= I2C_0
export TEST_SRF02_SPEED ?= I2C_SPEED_NORMAL
export TEST_MODE ?= SRF02_MODE_REAL_CM

include $(RIOTBASE)/Makefile.include

ifneq (,$(TEST_SRF02_I2C))
export CFLAGS += -DTEST_SRF02_I2C=$(TEST_SRF02_I2C)
endif
ifneq (,$(TEST_SRF02_SPEED))
export CFLAGS += -DTEST_SRF02_SPEED=$(TEST_SRF02_SPEED)
endif
ifneq (,$(TEST_MODE))
export CFLAGS += -DTEST_MODE=$(TEST_MODE)
endif
