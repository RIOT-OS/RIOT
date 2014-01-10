# name of your project
export PROJECT = rpl_udp

# for easy switching of boards
ifeq ($(strip $(BOARD)),)
	export BOARD = native
endif

# get rid of the mandatory RPL warning
CFLAGS += "-Wno-cpp"

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE =$(CURDIR)/../..

## Modules to include. 

USEMODULE += auto_init
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += uart0
USEMODULE += posix
USEMODULE += ps 
USEMODULE += vtimer 
ifeq ($(strip $(BOARD)),native)
	USEMODULE += nativenet
else
	USEMODULE += cc110x
endif
USEMODULE += sixlowpan
USEMODULE += rpl
USEMODULE += destiny

export INCLUDES += -I$(RIOTBASE)/sys/net/include -I$(RIOTBASE)/sys/net/routing/rpl -I$(RIOTBASE)/drivers/cc110x

include $(RIOTBASE)/Makefile.include
