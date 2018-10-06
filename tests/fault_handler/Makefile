# name of your application

ifeq ($(shell uname),Darwin)
CFLAGS += -Wno-language-extension-token
endif

USEMODULE += xtimer

include ../Makefile.tests_common
include $(RIOTBASE)/Makefile.include
