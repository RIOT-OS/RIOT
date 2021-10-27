include ../Makefile.tests_common

ifneq (,$(filter remote-revb,$(BOARD)))
  USEMODULE += cpp11-compat
endif

USEPKG += arduino_sdi_12

BOARD_WHITELIST := \
  arduino-leonardo \
  arduino-mega2560 \
  remote-revb \
  #

include $(RIOTBASE)/Makefile.include
