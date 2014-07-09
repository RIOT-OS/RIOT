export APPLICATION = olsr_node

export BOARD ?= native
export RIOTBASE ?= $(CURDIR)/../..

# other toolchains lack assert.h used by oonf_api
BOARD_WHITELIST := avsextrem msba2 native

USEPKG += oonf_api

export CFLAGS = -DRIOT -DENABLE_NAME

# Modules to include. 
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

# on native, use PID as node ID
ifeq ($(BOARD),native)
	export CFLAGS += -DUSE_PID
endif
ifeq ($(BOARD),msba2)
	export CFLAGS += -DENABLE_LEDS
endif

include $(RIOTBASE)/Makefile.include
