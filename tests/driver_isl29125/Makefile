APPLICATION = driver_isl29125
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += isl29125
USEMODULE += xtimer

ifneq (,$(TEST_ISL29125_I2C))
  CFLAGS += -DTEST_ISL29125_I2C=$(TEST_ISL29125_I2C)
else
  # set random default
  CFLAGS += -DTEST_ISL29125_I2C=I2C_0
endif
ifneq (,$(TEST_ISL29125_IRQ_PIN))
  CFLAGS += -DTEST_ISL29125_IRQ_PIN=$(TEST_ISL29125_IRQ_PIN)
else
  # set random default
  CFLAGS += -DTEST_ISL29125_IRQ_PIN=GPIO\(0,0\)
endif

include $(RIOTBASE)/Makefile.include
