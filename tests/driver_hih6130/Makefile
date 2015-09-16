APPLICATION = driver_hih6130
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += hih6130
USEMODULE += xtimer

ifneq (,$(TEST_HIH6130_I2C))
  CFLAGS += -DTEST_HIH6130_I2C=$(TEST_HIH6130_I2C)
else
  # set arbitrary default
  CFLAGS += -DTEST_HIH6130_I2C=I2C_0
endif
ifneq (,$(TEST_HIH6130_ADDR))
  CFLAGS += -DTEST_HIH6130_ADDR=$(TEST_HIH6130_ADDR)
else
  # factory default address
  CFLAGS += -DTEST_HIH6130_ADDR=0x27
endif

include $(RIOTBASE)/Makefile.include
