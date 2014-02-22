export PROJECT = test_nativenet
include ../Makefile.tests_common

ifeq (,$(filter native,$(BOARD)))
include $(RIOTBASE)/Makefile.unsupported
else

USEMODULE += auto_init
USEMODULE += hwtimer
USEMODULE += nativenet
USEMODULE += transceiver

include $(RIOTBASE)/Makefile.include

FORCE:
	touch main.c

sender: CFLAGS += -DSENDER
sender: PROJECT = test_nativenet_sender
sender: FORCE all

endif
