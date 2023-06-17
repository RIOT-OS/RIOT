RIOTBASE ?= $(CURDIR)/../../..
BOARD ?= atmega8
PORT ?= /dev/ttyACM0

include ../../Makefile.tests_common

USEMODULE += shell_cmds_default
#~ USEMODULE += ps
DEVELHELP = 0

CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE
DISABLE_MODULE += core_msg
CFLAGS += -DTHREAD_STACKSIZE_IDLE=74
CFLAGS += -DTHREAD_STACKSIZE_MAIN=150

include $(RIOTBASE)/Makefile.include
