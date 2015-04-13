####
#### Sample Makefile for building apps with the RIOT OS
####
#### The Sample Filesystem Layout is:
#### /this makefile
#### ../../RIOT 
#### 

# name of your project
export PROJECT =safest_demo_router

# for easy switching of boards
ifeq ($(strip $(BOARD)),)
	export BOARD = msba2
endif

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE =$(CURDIR)/../../RIOT

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

export INCLUDES = -I$(RIOTBOARD)/$(BOARD)/include -I$(RIOTBASE)/core/include -I$(RIOTCPU)/$(CPU)/include -I$(RIOTBASE)/sys/lib -I$(RIOTBASE)/sys/include/ -I$(RIOTBASE)/drivers/include/ -I$(RIOTBASE)/sys/net/include -I$(RIOTBASE)/sys/net/rpl -I$(RIOTBASE)/drivers/cc110x/

include $(RIOTBASE)/Makefile.include
