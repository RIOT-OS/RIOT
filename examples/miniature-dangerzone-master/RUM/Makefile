# name of your application
export APPLICATION = rum

# If no BOARD is found in the environment, use this default:
export BOARD ?= msba2 

# This has to be the absolute path to the RIOT base directory:
export RIOTBASE ?= $(CURDIR)/../../RIOT

CFLAGS += -DSCHEDSTATISTICS
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
export QUIET ?= 1

# get rid of the mandatory RPL warning
ifeq ($(shell $(CC) -Wno-cpp -E - 2>/dev/null >/dev/null dev/null ; echo $$?),0)
	ifeq ($(shell LANG=C $(CC) -Wno-cpp -E - 2>&1 1>/dev/null dev/null | grep warning: | grep -- -Wno-cpp),)
		CFLAGS += -Wno-cpp
	endif
endif

BOARD_INSUFFICIENT_RAM := chronos msb-430h redbee-econotag telosb wsn430-v1_3b wsn430-v1_4 z1
BOARD_BLACKLIST := arduino-due mbed_lpc1768 msb-430 pttu udoo qemu-i386
# mbed_lpc1768: see https://github.com/RIOT-OS/RIOT/issues/675
# msb-430: see https://github.com/RIOT-OS/RIOT/issues/658
# pttu: see https://github.com/RIOT-OS/RIOT/issues/659
# qemu-i386: no tranceiver, yet

# Modules to include:

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += posix
USEMODULE += ps
USEMODULE += vtimer
USEMODULE += defaulttransceiver
USEMODULE += rpl
USEMODULE += destiny
USEMODULE += ltc4150

include $(RIOTBASE)/Makefile.include

FORCE:
	touch main.c

root: CFLAGS += -DROOT
root: HEXFILE = rum_root.hex
root: FORCE all
