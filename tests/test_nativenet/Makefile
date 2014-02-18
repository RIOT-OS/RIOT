debug: CFLAGS += -g
debug: CFLAGS += -DENABLE_DEBUG

# name of your application
export PROJECT = test_nativenet
#
# for easy switching of boards
export BOARD = native

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE = $(CURDIR)/../..

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
