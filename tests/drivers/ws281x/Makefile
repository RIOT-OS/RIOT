BOARD ?= atmega328p
include ../Makefile.drivers_common

# Update this to your needs
# PIN ?= GPIO_PIN(0, 0)
# N ?= 8
# When using the ws281x_timer_gpio_ll module, you'll also need to define those:
# (Example values are suitable for nRF52 devices)
# TIMER ?= 2
# FREQ ?= 16000000

USEMODULE += ws281x
USEMODULE += xtimer

EXTERNAL_BOARD_DIRS += $(RIOTBASE)/tests/build_system/external_board_dirs/esp-ci-boards

include $(RIOTBASE)/Makefile.include

ifneq (, $(PIN))
  CFLAGS += '-DWS281X_PARAM_PIN=$(PIN)'
endif
ifneq (, $(N))
  CFLAGS += '-DWS281X_PARAM_NUMOF=$(N)'
endif
ifneq (, $(TIMER))
  CFLAGS += '-DWS281X_TIMER_DEV=TIMER_DEV($(TIMER))' '-DWS281X_TIMER_MAX_VALUE=TIMER_$(TIMER)_MAX_VALUE'
endif
ifneq (, $(FREQ))
  CFLAGS += '-DWS281X_TIMER_FREQ=$(FREQ)'
endif
