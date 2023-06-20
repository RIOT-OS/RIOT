RIOTBASE ?= $(CURDIR)/../../..
BOARD ?= atmega8
PORT ?= /dev/ttyACM0

include ../../Makefile.tests_common

FEATURES_REQUIRED += periph_spi

DEVELHELP = 0
CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE
DISABLE_MODULE += core_msg

include $(RIOTBASE)/Makefile.include
