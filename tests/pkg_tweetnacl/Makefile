include ../Makefile.tests_common

TEST_ON_CI_WHITELIST += native

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(5*THREAD_STACKSIZE_DEFAULT\)

USEPKG += tweetnacl
USEMODULE += embunit

BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-mega2560 \
  arduino-uno \
  mega-xplained \
  nucleo-f031k6 \
  nucleo-f042k6 \
  #

include $(RIOTBASE)/Makefile.include
