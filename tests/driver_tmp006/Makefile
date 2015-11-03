APPLICATION = driver_tmp006
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += tmp006
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_TMP006_I2C       ?= I2C_0
TEST_TMP006_ADDR      ?= 0x41
TEST_TMP006_CONFIG_CR ?= TMP006_CONFIG_CR_DEF

# export parameters
CFLAGS += -DTEST_TMP006_I2C=$(TEST_TMP006_I2C)
CFLAGS += -DTEST_TMP006_ADDR=$(TEST_TMP006_ADDR)
CFLAGS += -DTEST_TMP006_CONFIG_CR=$(TEST_TMP006_CONFIG_CR)

include $(RIOTBASE)/Makefile.include
