APPLICATION = driver_pir
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_gpio

# Define default pin mappings for some boards:
ifneq (,$(filter stm32f4discovery,$(BOARD)))
  export PIR_GPIO ?= GPIO\(3,7\)
endif
ifneq (,$(filter arduino-due,$(BOARD)))
  export PIR_GPIO ?= GPIO\(0,20\)
endif

USEMODULE += pir
USEMODULE += vtimer

ifneq (,$(PIR_GPIO))
  CFLAGS += -DPIR_GPIO=$(PIR_GPIO)
else
  # set random default
  CFLAGS += -DPIR_GPIO=GPIO\(0,0\)
endif

include $(RIOTBASE)/Makefile.include

all-polling: CFLAGS += -DTEST_PIR_POLLING

all-polling: all

all-interrupt: all
