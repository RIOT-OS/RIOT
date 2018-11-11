include ../Makefile.tests_common

USEMODULE += hashes
USEMODULE += bloom
USEMODULE += random
USEMODULE += xtimer

DISABLE_MODULE += auto_init

TEST_ON_CI_WHITELIST += all

BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-uno \
  chronos \
  msb-430 \
  msb-430h \
  telosb \
  wsn430-v1_3b \
  wsn430-v1_4 \
  z1 \
  #

include $(RIOTBASE)/Makefile.include
