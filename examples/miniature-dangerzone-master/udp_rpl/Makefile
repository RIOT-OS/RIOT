####
#### Sample Makefile for building apps with the RIOT OS
####
#### The Sample Filesystem Layout is:
#### /this makefile
#### ../../RIOT 
#### ../../boards   for board definitions (if you have one or more)
#### 

# name of your project
export PROJECT =foobar

# for easy switching of boards
ifeq ($(strip $(BOARD)),)
	export BOARD = native
endif

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE =$(CURDIR)/../../RIOT
export RIOTCPU =$(CURDIR)/../../RIOT/cpu
export RIOTBOARD =$(CURDIR)/../../boards

## Modules to include. 

USEMODULE += shell
USEMODULE += uart0
USEMODULE += posix
#USEMODULE += vtimer
#USEMODULE += sht11
#USEMODULE += ltc4150
#USEMODULE += cc110x
#USEMODULE += fat
USEMODULE += sixlowpan
USEMODULE += nativenet
USEMODULE += transceiver
USEMODULE += vtimer
USEMODULE += auto_init

export INCLUDES = -I$(RIOTBOARD)/$(BOARD)/include -I$(RIOTBASE)/core/include -I$(RIOTCPU)/$(CPU)/include -I$(RIOTBASE)/sys/lib -I$(RIOTBASE)/sys/include/ -I$(RIOTBASE)/drivers/include/ -I$(RIOTBASE)/sys/net/sixlowpan/include

include $(RIOTBASE)/Makefile.include
