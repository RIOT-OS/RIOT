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

# on msba2 and avsextrem cpuid_get is not implemented
ifneq (,$(filter $(BOARD), msba2 avsextrem))
	export CFLAGS += -DENABLE_LEDS -DHAVE_NO_CPUID
endif

include $(RIOTBASE)/Makefile.include
