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
    openlabs-kw41z-mini \
    frdm-k64f \
    frdm-k22f \
    #

BOARDS_TIMER_16MHz := \
  cc2538dk \
  openmote-b \
  openmote-cc2538 \
  remote-reva \
  remote-revb \
  #

ifneq (,$(filter $(BOARDS_TIMER_25kHz),$(BOARD)))
  TIMER_SPEED ?= 250000
else ifneq (,$(filter $(BOARDS_TIMER_32kHz),$(BOARD)))
  TIMER_SPEED ?= 32768
else ifneq (,$(filter $(BOARDS_TIMER_16MHz),$(BOARD)))
  TIMER_SPEED ?= 16000000
endif

TIMER_SPEED ?= 1000000

CFLAGS += -DTIMER_SPEED=$(TIMER_SPEED)
include $(RIOTBASE)/Makefile.include
