include ../Makefile.tests_common

USEPKG += libhydrogen
USEMODULE += embunit

# AVR boards: require avr-gcc >= 7.0 (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60040)
# MSP430 boards: invalid alignment of 'hydro_random_context'
BOARD_BLACKLIST := \
  arduino-duemilanove \
  arduino-leonardo \
  arduino-mega2560 \
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
