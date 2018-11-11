include ../Makefile.tests_common

DISABLE_MODULE += auto_init

USEMODULE += shell
USEMODULE += timex

TEST_ON_CI_WHITELIST += all

BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-uno \
  #

# The MSP-430 toolchain lacks sscanf:
BOARD_BLACKLIST := \
  chronos \
  msb-430 \
  msb-430h \
  telosb \
  wsn430-v1_3b \
  wsn430-v1_4 \
  z1 \
  #

include $(RIOTBASE)/Makefile.include
