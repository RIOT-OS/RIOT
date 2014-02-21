####
#### Sample Makefile for building apps with the RIOT OS
####
#### The Sample Filesystem Layout is:
#### /this makefile
#### ../../RIOT
#### ../../boards   for board definitions (if you have one or more)
####

export PROJECT =test_pnet
include ../Makefile.tests_common

ifeq ($(BOARD),stm32f4discovery)
	include Makefile.$(BOARD)
endif

## Modules to include.

USEMODULE += auto_init
USEMODULE += posix
USEMODULE += pnet
USEMODULE += vtimer
ifeq ($(strip $(BOARD)),native)
	USEMODULE += nativenet
else ifeq ($(strip $(BOARD)),msba2)
	USEMODULE += cc110x_ng
endif

ifeq ($(BOARD),native)
	CFLAGS += -isystem $(RIOTBASE)/sys/net/include \
			  -isystem $(RIOTBASE)/sys/posix/pnet/include
else
	export INCLUDES += -I$(RIOTBASE)/sys/net/include \
					   -I$(RIOTBASE)/sys/posix/pnet/include \
					   -I$(RIOTBASE)/sys/posix/include
endif

include $(RIOTBASE)/Makefile.include
