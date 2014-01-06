# name of your project
export PROJECT = ccn-lite-client

# for easy switching of boards
ifeq ($(strip $(BOARD)),)
	export BOARD = native
endif

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE = $(CURDIR)/../..

# uncomment this to enable scheduler statistics for ps
#CFLAGS += -DSCHEDSTATISTICS

# If you want to use valgrind, you should recompile native with either
# HAVE_VALGRIND_H or HAVE_VALGRIND_VALGRIND_H depending on the location
# of the valgrind header (i.e. <valgrind.h> or <valgrind/valgrind.h>)
# For more information about the valgrind support of RIOT read:
# RIOT/cpu/native/README
#CFLAGS += -DHAVE_VALGRIND_VALGRIND_H
#CFLAGS += -DHAVE_VALGRIND_H

## Modules to include. 

USEMODULE += config
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += uart0
USEMODULE += posix
USEMODULE += ps
USEMODULE += auto_init
USEMODULE += hwtimer
USEMODULE += ltc4150

USEMODULE += transceiver
ifeq ($(BOARD),msba2)
	USEMODULE += cc110x_ng
else ifeq ($(BOARD),native)
	USEMODULE += nativenet
endif

USEMODULE += rtc
USEMODULE += crypto_sha256
USEMODULE += ccn_lite
USEMODULE += ccn_lite_client

export INCLUDES =  -I$(RIOTBASE)/drivers/cc110x_ng/include/ -I$(RIOTBASE)/sys/net/include

include $(RIOTBASE)/Makefile.include
