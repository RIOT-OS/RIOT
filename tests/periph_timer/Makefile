include ../Makefile.tests_common

FEATURES_REQUIRED = periph_timer

BOARDS_TIMER_25kHz := \
    arduino-duemilanove \
    arduino-leonardo \
    arduino-mega2560 \
    arduino-uno \
    atmega256rfr2-xpro \
    atmega328p \
    waspmote-pro \
    #

BOARDS_TIMER_32kHz := \
    hifive1 \
    hifive1b \
    %-kw41z \
    frdm-k64f \
    frdm-k22f \
    #

ifneq (,$(filter $(BOARDS_TIMER_25kHz),$(BOARD)))
  TIMER_SPEED ?= 250000
else ifneq (,$(filter $(BOARDS_TIMER_32kHz),$(BOARD)))
  TIMER_SPEED ?= 32768
endif

TIMER_SPEED ?= 1000000

CFLAGS += -DTIMER_SPEED=$(TIMER_SPEED)
include $(RIOTBASE)/Makefile.include
