include ../Makefile.tests_common

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(4*THREAD_STACKSIZE_DEFAULT\)

USEPKG += qDSA
USEMODULE += random
USEMODULE += embunit

# qDSA is not 16 bit compatible
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
