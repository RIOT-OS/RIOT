APPLICATION = nativenet
include ../Makefile.tests_common

BOARD_WHITELIST := native

USEMODULE += nativenet
USEMODULE += transceiver
USEMODULE += nomac

INCLUDES += -I$(RIOTBASE)/sys/net/include
CFLAGS += -DHAS_NETAPI_PID

include $(RIOTBASE)/Makefile.include

FORCE:
	touch main.c

sender: CFLAGS += -DSENDER
sender: APPLICATION = nativenet_sender
sender: FORCE all

test:
	./tests/01-tests.py
