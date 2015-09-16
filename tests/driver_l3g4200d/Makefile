APPLICATION = driver_l3g4200d
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c periph_gpio

USEMODULE += l3g4200d
USEMODULE += xtimer

ifneq (,$(TEST_L3G4200D_I2C))
  CFLAGS += -DTEST_L3G4200D_I2C=$(TEST_L3G4200D_I2C)
else
  # set random default
  CFLAGS += -DTEST_L3G4200D_I2C=I2C_0
endif
ifneq (,$(TEST_L3G4200D_ADDR))
  CFLAGS += -DTEST_L3G4200D_ADDR=$(TEST_L3G4200D_ADDR)
else
  # set random default
  CFLAGS += -DTEST_L3G4200D_ADDR=104
endif
ifneq (,$(TEST_L3G4200D_INT))
  CFLAGS += -DTEST_L3G4200D_INT=$(TEST_L3G4200D_INT)
else
  # set random default
  CFLAGS += -DTEST_L3G4200D_INT=GPIO\(0,0\)
endif
ifneq (,$(TEST_L3G4200D_DRDY))
  CFLAGS += -DTEST_L3G4200D_DRDY=$(TEST_L3G4200D_DRDY)
else
  # set random default
  CFLAGS += -DTEST_L3G4200D_DRDY=GPIO\(0,1\)
endif

include $(RIOTBASE)/Makefile.include
