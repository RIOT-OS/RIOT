include ../Makefile.periph_common

FEATURES_REQUIRED := periph_timer
FEATURES_REQUIRED += periph_timer_capture
FEATURES_REQUIRED += periph_timer_query_freqs
FEATURES_REQUIRED += periph_gpio_ll

USEMODULE += tiny_strerror

# Custom per-board pin configuration (e.g. for setting PORT_IN, PIN_IN_0, ...)
# can be provided in a Makefile.$(BOARD) file:
-include Makefile.$(BOARD)

###################################
# Variables to configure the test #
###################################
TIMER_CAPTURE_DEV ?= 0
TIMER_COMPARE_DEV ?= $(TIMER_CAPTURE_DEV)
TRIGGER_GPIO_PORT ?= 1
TRIGGER_GPIO_PIN ?= 0
TINY_DELAY_US ?= 20

include $(RIOTBASE)/Makefile.include

CFLAGS += '-DCONFIG_TIMER_CAPTURE_DEV=$(TIMER_CAPTURE_DEV)'
CFLAGS += '-DCONFIG_TIMER_COMPARE_DEV=$(TIMER_COMPARE_DEV)'
CFLAGS += '-DCONFIG_TRIGGER_GPIO_PORT=$(TRIGGER_GPIO_PORT)'
CFLAGS += '-DCONFIG_TRIGGER_GPIO_PIN=$(TRIGGER_GPIO_PIN)'
CFLAGS += '-DCONFIG_TINY_DELAY_US=$(TINY_DELAY_US)'
