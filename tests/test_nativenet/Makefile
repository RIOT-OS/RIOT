export APPLICATION = test_nativenet
include ../Makefile.tests_common

BOARD_WHITELIST := native

ifeq (,$(filter native,$(BOARD)))
include $(RIOTBASE)/Makefile.unsupported
else

USEMODULE += nativenet
USEMODULE += transceiver

include $(RIOTBASE)/Makefile.include

FORCE:
	touch main.c

sender: CFLAGS += -DSENDER
sender: APPLICATION = test_nativenet_sender
sender: FORCE all

endif
