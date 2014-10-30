APPLICATION = driver_lps331ap
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += lps331ap
USEMODULE += vtimer

ifneq (,$(TEST_LPS331AP_I2C))
  CFLAGS += -DTEST_LPS331AP_I2C=$(TEST_LPS331AP_I2C)
else
  # set random default
  CFLAGS += -DTEST_LPS331AP_I2C=I2C_0
endif
ifneq (,$(TEST_LPS331AP_ADDR))
  CFLAGS += -DTEST_LPS331AP_ADDR=$(TEST_LPS331AP_ADDR)
else
  # set random default
  CFLAGS += -DTEST_LPS331AP_ADDR=92
endif

include $(RIOTBASE)/Makefile.include
