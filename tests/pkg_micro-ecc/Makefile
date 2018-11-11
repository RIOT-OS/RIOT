include ../Makefile.tests_common

USEMODULE += hashes
USEPKG += micro-ecc

TEST_ON_CI_WHITELIST += all

BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-uno \
  #

# micro-ecc is not 16 bit compatible
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
