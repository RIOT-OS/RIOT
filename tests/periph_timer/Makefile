include ../Makefile.tests_common

FEATURES_REQUIRED = periph_timer

BOARDS_TIMER_500kHz := \
    atxmega-a1u-xpro \
    #

BOARDS_TIMER_250kHz := \
    arduino-duemilanove \
    arduino-leonardo \
    arduino-mega2560 \
    arduino-uno \
    atmega256rfr2-xpro \
    atmega328p \
    #

BOARDS_TIMER_32kHz := \
    e180-zg120b-tb \
    hifive1 \
    hifive1b \
    ikea-tradfri \
    %-kw41z \
    openlabs-kw41z-mini \
    frdm-k64f \
    frdm-k22f \
    slstk3400a \
    slstk3401a \
    slstk3402a \
    sltb001a \
    slwstk6000b-% \
    slwstk6220a \
    stk3600 \
    stk3700 \
    #

BOARDS_TIMER_CLOCK_CORECLOCK := \
  cc2538dk \
  openmote-b \
  openmote-cc2538 \
  remote-reva \
  remote-revb \
  waspmote-pro \
  #

ifneq (,$(filter $(BOARDS_TIMER_500kHz),$(BOARD)))
  TIMER_SPEED ?= 500000
else ifneq (,$(filter $(BOARDS_TIMER_250kHz),$(BOARD)))
  TIMER_SPEED ?= 250000
else ifneq (,$(filter $(BOARDS_TIMER_32kHz),$(BOARD)))
  TIMER_SPEED ?= 32768
else ifneq (,$(filter $(BOARDS_TIMER_CLOCK_CORECLOCK),$(BOARD)))
  TIMER_SPEED ?= CLOCK_CORECLOCK
endif

TIMER_SPEED ?= 1000000

CFLAGS += -DTIMER_SPEED=$(TIMER_SPEED)
include $(RIOTBASE)/Makefile.include
