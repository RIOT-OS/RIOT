APPLICATION = driver_srf02
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += xtimer
USEMODULE += srf02

# Define default settings
export TEST_SRF02_I2C ?= I2C_0
export TEST_MODE ?= SRF02_MODE_REAL_CM

include $(RIOTBASE)/Makefile.include

ifneq (,$(TEST_SRF02_I2C))
export CFLAGS += -DTEST_SRF02_I2C=$(TEST_SRF02_I2C)
endif
ifneq (,$(TEST_MODE))
export CFLAGS += -DTEST_MODE=$(TEST_MODE)
endif
