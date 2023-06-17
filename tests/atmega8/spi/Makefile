RIOTBASE ?= $(CURDIR)/../../..
BOARD ?= atmega8
PORT ?= /dev/ttyACM0

include ../../Makefile.tests_common

FEATURES_REQUIRED += periph_spi

DEVELHELP = 0
CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE
DISABLE_MODULE += core_msg
CFLAGS += -DTHREAD_STACKSIZE_IDLE=64
CFLAGS += -DTHREAD_STACKSIZE_MAIN=128

include $(RIOTBASE)/Makefile.include
