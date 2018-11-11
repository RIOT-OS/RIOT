include ../Makefile.tests_common

TEST_ON_CI_WHITELIST += all

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(5*THREAD_STACKSIZE_DEFAULT\)
USEPKG += hacl
USEMODULE += random
USEMODULE += embunit

BOARD_BLACKLIST := \
  arduino-duemilanove \
  arduino-mega2560 \
  arduino-uno \
  chronos \
  jiminy-mega256rfr2 \
  mega-xplained \
  msb-430 \
  msb-430h \
  telosb \
  waspmote-pro \
  wsn430-v1_3b \
  wsn430-v1_4 \
  z1 \
  #

BOARD_INSUFFICIENT_MEMORY := \
  nucleo-f031k6 \
  nucleo-f042k6 \
  #

include $(RIOTBASE)/Makefile.include
