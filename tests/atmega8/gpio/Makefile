RIOTBASE ?= $(CURDIR)/../../..
BOARD ?= atmega8
PORT ?= /dev/ttyACM0

include ../../Makefile.tests_common

FEATURES_REQUIRED += periph_gpio

USEMODULE += shell

CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE
DISABLE_MODULE += core_msg
DEVELHELP = 0

include $(RIOTBASE)/Makefile.include
