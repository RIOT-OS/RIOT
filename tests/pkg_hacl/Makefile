include ../Makefile.tests_common

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(5*THREAD_STACKSIZE_DEFAULT\)
USEPKG += hacl
USEMODULE += random
USEMODULE += embunit

BOARD_BLACKLIST := \
  arduino-duemilanove \
  arduino-leonardo \
  arduino-mega2560 \
  arduino-nano \
  arduino-uno \
  atmega328p \
  chronos \
  mega-xplained \
  microduino-corerf \
  msb-430 \
  msb-430h \
  telosb \
  waspmote-pro \
  wsn430-v1_3b \
  wsn430-v1_4 \
  z1 \
  #

include $(RIOTBASE)/Makefile.include
