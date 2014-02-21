debug: CFLAGS += -g
debug: CFLAGS += -DENABLE_DEBUG

export PROJECT = test_nativenet
include ../Makefile.tests_common

ifeq (,$(filter native,$(BOARD)))
include $(RIOTBASE)/Makefile.unsupported
else

## Modules to include.

USEMODULE += auto_init
USEMODULE += hwtimer
USEMODULE += nativenet
USEMODULE += transceiver

include $(RIOTBASE)/Makefile.include
debug: all

FORCE:
	touch main.c

sender: CFLAGS += -DSENDER
sender: PROJECT = test_nativenet_sender
sender: FORCE all

endif
