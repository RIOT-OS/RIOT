# name of your project
export PROJECT = test_pthread

# for easy switching of boards
export BOARD ?= native

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE = $(CURDIR)/../..

## Modules to include.
USEMODULE += posix
USEMODULE += pthread

ifeq ($(BOARD),native)
	CFLAGS += -isystem $(RIOTBASE)/sys/posix/pthread/include
else
	export INCLUDES += -I$(RIOTBASE)/sys/posix/pthread/include \
					   -I$(RIOTBASE)/sys/posix/include
endif

include $(RIOTBASE)/Makefile.include

