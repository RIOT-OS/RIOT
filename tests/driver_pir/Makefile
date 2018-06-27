include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

FEATURES_REQUIRED = periph_gpio

USEMODULE += pir
USEMODULE += xtimer

# define parameters for selected boards
ifneq (,$(filter stm32f4discovery,$(BOARD)))
  PIR_PARAM_GPIO ?= GPIO_PIN\(3,7\)
  PIR_PARAM_ACTIVE_HIGH ?= 1
endif
ifneq (,$(filter arduino-due,$(BOARD)))
  PIR_PARAM_GPIO ?= GPIO_PIN\(0,20\)
  PIR_PARAM_ACTIVE_HIGH ?= 1
endif

# set default device parameters in case they are undefined
PIR_PARAM_GPIO ?= GPIO_PIN\(0,0\)
PIR_PARAM_ACTIVE_HIGH ?= 1

# export parameters
CFLAGS += -DPIR_PARAM_GPIO=$(PIR_PARAM_GPIO)
CFLAGS += -DPIR_PARAM_ACTIVE_HIGH=$(PIR_PARAM_ACTIVE_HIGH)

include $(RIOTBASE)/Makefile.include

all-polling: CFLAGS += -DTEST_PIR_POLLING

all-polling: all

all-interrupt: all
