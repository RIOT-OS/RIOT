# name of your application
APPLICATION = fault_handler
include ../Makefile.tests_common

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

CFLAGS += -DDEVELHELP=1

ifeq ($(shell uname),Darwin)
CFLAGS += -Wno-language-extension-token
endif

USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
