APPLICATION = riotboot

BOARD ?= samr21-xpro

USEMODULE += firmware

CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE
DISABLE_MODULE += auto_init

RIOTBASE ?= $(CURDIR)/../..

include $(RIOTBASE)/Makefile.include
