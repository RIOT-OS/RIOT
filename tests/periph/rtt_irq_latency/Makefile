BOARD ?= nucleo-l452re
include ../Makefile.periph_common

FEATURES_REQUIRED = periph_rtt
FEATURES_REQUIRED += periph_timer
FEATURES_OPTIONAL += periph_rtt_overflow
FEATURES_OPTIONAL += periph_rtt_set_counter
FEATURES_OPTIONAL += periph_timer_query_freqs

USEMODULE += embunit

DISABLE_MODULE += periph_init_rtt

include $(RIOTBASE)/Makefile.include
