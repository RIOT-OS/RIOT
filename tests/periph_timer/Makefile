include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno

FEATURES_REQUIRED = periph_timer

TEST_ON_CI_WHITELIST += all

ifneq (,$(filter arduino-duemilanove arduino-mega2560 arduino-uno waspmote-pro,$(BOARD)))
  TIMER_SPEED ?= 250000
else ifneq (,$(filter hifive1,$(BOARD)))
  TIMER_SPEED ?= 32768
endif

TIMER_SPEED ?= 1000000

CFLAGS += -DTIMER_SPEED=$(TIMER_SPEED)
include $(RIOTBASE)/Makefile.include
