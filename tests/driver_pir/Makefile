APPLICATION = driver_pir
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_gpio

USEMODULE += pir
USEMODULE += xtimer

# define parameters for selected boards
ifneq (,$(filter stm32f4discovery,$(BOARD)))
  PIR_GPIO ?= GPIO_PIN\(3,7\)
endif
ifneq (,$(filter arduino-due,$(BOARD)))
  PIR_GPIO ?= GPIO_PIN\(0,20\)
endif

# set default device parameters in case they are undefined
PIR_GPIO ?= GPIO_PIN\(0,0\)

# export parameters
CFLAGS += -DPIR_GPIO=$(PIR_GPIO)

include $(RIOTBASE)/Makefile.include

all-polling: CFLAGS += -DTEST_PIR_POLLING

all-polling: all

all-interrupt: all
