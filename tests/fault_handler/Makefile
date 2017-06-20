# name of your application
APPLICATION = fault_handler
include ../Makefile.tests_common

CFLAGS += -DDEVELHELP=1

ifeq ($(shell uname),Darwin)
CFLAGS += -Wno-language-extension-token
endif

USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
