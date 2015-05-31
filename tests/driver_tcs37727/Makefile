APPLICATION = driver_tcs37727
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += tcs37727
USEMODULE += xtimer

ifneq (,$(TEST_TCS37727_I2C))
  CFLAGS += -DTEST_TCS37727_I2C=$(TEST_TCS37727_I2C)
else
  # set random default
  CFLAGS += -DTEST_TCS37727_I2C=I2C_0
endif
ifneq (,$(TEST_TCS37727_ADDR))
  CFLAGS += -DTEST_TCS37727_ADDR=$(TEST_TCS37727_ADDR)
else
  # set random default
  CFLAGS += -DTEST_TCS37727_ADDR=0x29
endif

include $(RIOTBASE)/Makefile.include
