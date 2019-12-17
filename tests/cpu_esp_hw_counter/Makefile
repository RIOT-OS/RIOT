BOARD ?= esp32-wroom-32

include ../Makefile.tests_common

FEATURES_REQUIRED += arch_esp32
FEATURES_REQUIRED += periph_timer

USEMODULE += esp_hw_counter

TIMER_SPEED ?= 1000000

CFLAGS += -DTIMER_SPEED=$(TIMER_SPEED)
include $(RIOTBASE)/Makefile.include
