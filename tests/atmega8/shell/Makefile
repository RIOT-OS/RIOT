RIOTBASE ?= $(CURDIR)/../../..
BOARD ?= atmega8
PORT ?= /dev/ttyACM0

include ../../Makefile.tests_common

USEMODULE += shell_cmds_default
USEMODULE += ps
DEVELHELP = 1

CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE
DISABLE_MODULE += core_msg

include $(RIOTBASE)/Makefile.include
