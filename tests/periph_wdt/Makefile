BOARD ?= nucleo-l152re
include ../Makefile.tests_common

# Use socat so '\n' are not ignored when waiting for '>'
RIOT_TERMINAL ?= socat

FEATURES_REQUIRED += periph_wdt

USEMODULE += xtimer
USEMODULE += shell

include $(RIOTBASE)/Makefile.include
