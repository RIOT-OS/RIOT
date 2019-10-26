include ../Makefile.tests_common


USEMODULE += embunit

USEMODULE += crypto
USEMODULE += cipher_modes
CFLAGS += -DCRYPTO_THREEDES

# Issue with integer width
BOARD_BLACKLIST += \
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
