BOARD ?= nucleo-f767zi
include ../Makefile.tests_common

FEATURES_REQUIRED += periph_ptp
FEATURES_OPTIONAL += periph_ptp_speed_adjustment
FEATURES_REQUIRED += periph_timer_periodic

USEMODULE += fmt

TIMER_FREQ ?=

include $(RIOTBASE)/Makefile.include

ifneq (,$(TIMER_FREQ))
  CFLAGS += -DTIM_FREQ=$(TIMER_FREQ)
endif
