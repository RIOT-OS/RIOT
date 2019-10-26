include ../Makefile.tests_common

USEMODULE += libfixmath-unittests

ifneq (,$(filter native,$(BOARD)))
  export LINKFLAGS += -lm
endif

USEMODULE += printf_float

TEST_ON_CI_WHITELIST += native

BOARD_BLACKLIST := \
  arduino-duemilanove \
  arduino-leonardo \
  arduino-mega2560 \
  arduino-nano \
  arduino-uno \
  atmega328p \
  mega-xplained \
  microduino-corerf \
  waspmote-pro \
  #

# arduino-mega2560: builds locally but breaks travis (possibly because of
# differences in the toolchain)
# The MSP boards don't feature round(), exp(), and log(), which are used in the unittests
BOARD_BLACKLIST += \
  chronos \
  msb-430 \
  msb-430h \
  telosb \
  wsn430-v1_3b \
  wsn430-v1_4 \
  z1 \
  #

include $(RIOTBASE)/Makefile.include
