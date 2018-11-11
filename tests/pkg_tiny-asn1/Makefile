include ../Makefile.tests_common

USEPKG += tiny-asn1

TEST_ON_CI_WHITELIST += all

BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-uno \
  #

BOARD_BLACKLIST := \
  wsn430-v1_3b \
  wsn430-v1_4 \
  #

include $(RIOTBASE)/Makefile.include
