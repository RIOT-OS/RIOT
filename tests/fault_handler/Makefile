# name of your application
include ../Makefile.tests_common

ifeq ($(shell uname),Darwin)
CFLAGS += -Wno-language-extension-token
endif

USEMODULE += ztimer_usec

include $(RIOTBASE)/Makefile.include
