####
#### Sample Makefile for building apps with the RIOT OS
####
#### The Sample Filesystem Layout is:
#### /this makefile
#### ../../RIOT
#### ../../boards   for board definitions (if you have one or more)
####

# name of your project
export PROJECT = test_net_if

BOARD_WHITELIST = native msba2 telosb

# for easy switching of boards
ifeq ($(strip $(BOARD)),)
	export BOARD = native
endif

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE = $(CURDIR)/../../

ifeq ($(BOARD),stm32f4discovery)
	include Makefile.$(BOARD)
endif

## Modules to include.

USEMODULE += net_if
ifeq ($(BOARD),native)
	USEMODULE += nativenet
else ifeq ($(BOARD),msba2)
	USEMODULE += cc110x_ng
	USEMODULE += auto_init
else ifeq ($(BOARD),telosb)
	USEMODULE += cc2420
	USEMODULE += auto_init
else ifeq ($(BOARD),wsn430-v1_4)
	USEMODULE += cc2420
	USEMODULE += auto_init
endif

export INCLUDES += -I$(RIOTBASE)/sys/net/include

ifeq ($(BOARD),msba2)
	export INCLUDES += -I$(RIOTBASE)/drivers/cc110x_ng/include \
					   -I$(RIOTBASE)/boards/msba2-common/include
endif

include $(RIOTBASE)/Makefile.include
