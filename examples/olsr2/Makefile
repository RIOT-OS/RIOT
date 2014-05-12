####
#### Sample Makefile for building apps with the RIOT OS
####
#### The Sample Filesystem Layout is:
#### /this makefile
#### ../../RIOT 
#### ../../boards   for board definitions (if you have one or more)
#### 

# name of your application
export APPLICATION = olsr_node

export BOARD ?= native
export RIOTBASE ?= $(CURDIR)/../..

USEPKG += oonf_api

export CFLAGS = -DRIOT -DENABLE_NAME

## Modules to include. 

USEMODULE += rtc
USEMODULE += uart0
USEMODULE += posix
USEMODULE += ps
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += random
USEMODULE += config
USEMODULE += olsr2
USEMODULE += defaulttransceiver

ifeq ($(BOARD),native)
	export CFLAGS += -DUSE_PID
endif
ifeq ($(BOARD),msba2)
	export CFLAGS += -DENABLE_LEDS
endif

include $(RIOTBASE)/Makefile.include
