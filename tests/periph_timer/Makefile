include ../Makefile.tests_common

FEATURES_REQUIRED = periph_timer

TEST_ON_CI_WHITELIST += all

ifneq (,$(filter arduino-duemilanove arduino-leonardo arduino-mega2560 arduino-uno waspmote-pro,$(BOARD)))
  TIMER_SPEED ?= 250000
else ifneq (,$(filter hifive1 %-kw41z,$(BOARD)))
  TIMER_SPEED ?= 32768
endif

TIMER_SPEED ?= 1000000

CFLAGS += -DTIMER_SPEED=$(TIMER_SPEED)
include $(RIOTBASE)/Makefile.include
