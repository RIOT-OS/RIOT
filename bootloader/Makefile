APPLICATION = bootloader
BOARD ?= samr21-xpro

USEMODULE += firmware

CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE
DISABLE_MODULE += auto_init

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/..

include $(RIOTBASE)/Makefile.include
