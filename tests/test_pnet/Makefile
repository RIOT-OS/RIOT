####
#### Sample Makefile for building apps with the RIOT OS
####
#### The Sample Filesystem Layout is:
#### /this makefile
#### ../../RIOT 
#### ../../boards   for board definitions (if you have one or more)
#### 

# name of your project
export PROJECT =test_pnet

# for easy switching of boards
ifeq ($(strip $(BOARD)),)
	export BOARD = native
endif

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE =$(CURDIR)/../..
export RIOTBOARD =$(RIOTBASE)/boards

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

ifeq ($(BOARD),msba2)
	export INCLUDES += -I$(RIOTBASE)/drivers/cc110x_ng/include \
					   -I$(RIOTBASE)/boards/msba2-common/include
endif

include $(RIOTBASE)/Makefile.include
