APPLICATION = driver_lps25h
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += lps25h
USEMODULE += xtimer

ifneq (,$(TEST_LPS25H_I2C))
  CFLAGS += -DTEST_LPS25H_I2C=$(TEST_LPS25H_I2C)
else
  # set random default
  CFLAGS += -DTEST_LPS25H_I2C=I2C_1
endif
ifneq (,$(TEST_LPS25H_ADDR))
  CFLAGS += -DTEST_LPS25H_ADDR=$(TEST_LPS25H_ADDR)
else
  # set random default
  CFLAGS += -DTEST_LPS25H_ADDR=92
endif

include $(RIOTBASE)/Makefile.include
