include ../Makefile.pkg_common

USEPKG += zboss

# Blacklist because some esp boards complain about unknown type name pthread_condattr_t
# whereas the rest complains that _IOLBF is undeclared (used in zb_trace_riot.c)
BOARD_BLACKLIST = \
  esp32-ethernet-kit-v1_0 \
  esp32-ethernet-kit-v1_1 \
  esp32-ethernet-kit-v1_2 \
  esp32-olimex-evb \
  nucleo-l011k4 \
  samd10-xmini \
  stm32f030f4-demo

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += ztimer_sec

include $(RIOTBASE)/Makefile.include
