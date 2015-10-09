APPLICATION = driver_hdc1000
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += hdc1000
USEMODULE += xtimer

ifneq (,$(TEST_HDC1000_I2C))
  CFLAGS += -DTEST_HDC1000_I2C=$(TEST_HDC1000_I2C)
else
  # set random default
  CFLAGS += -DTEST_HDC1000_I2C=I2C_0
endif
ifneq (,$(TEST_HDC1000_ADDR))
  CFLAGS += -DTEST_HDC1000_ADDR=$(TEST_HDC1000_ADDR)
else
  # set random default
  CFLAGS += -DTEST_HDC1000_ADDR=0x43
endif

include $(RIOTBASE)/Makefile.include
