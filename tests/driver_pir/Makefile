APPLICATION = driver_pir
include ../Makefile.tests_common

# These boards do not suport periph/gpio.h at the time of this writing:
BOARD_BLACKLIST = native arduino-mega2560 avsextrem chronos mbed_lpc1768 msb-430 msb-430h msba2 pttu qemu-i386 redbee-econotag telosb wsn430-v1_3b wsn430-v1_4 z1

# Define default pin mappings for some boards:
BOARD_WHITELIST = stm32f4discovery arduino-due
ifneq (,$(filter stm32f4discovery,$(BOARD)))
  export PIR_GPIO ?= GPIO_8
endif
ifneq (,$(filter arduino-due,$(BOARD)))
  export PIR_GPIO ?= GPIO_10
endif

USEMODULE += pir
USEMODULE += vtimer

ifneq (,$(PIR_GPIO))
  CFLAGS += -DPIR_GPIO=$(PIR_GPIO)
endif

include $(RIOTBASE)/Makefile.include

all-polling: CFLAGS += -DTEST_PIR_POLLING

all-polling: all

all-interrupt: all
