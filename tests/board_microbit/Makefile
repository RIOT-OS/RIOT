BOARD ?= microbit

include ../Makefile.tests_common

# We want to test the microbit support module
USEMODULE += microbit

# This test application is for the BBC micro:bit only
BOARD_WHITELIST := \
  microbit \
  #

include $(RIOTBASE)/Makefile.include
