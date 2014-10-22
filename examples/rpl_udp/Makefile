# name of your application
APPLICATION = rpl_udp

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Uncomment these lines if you want to use platform support from external
# repositories:
#RIOTCPU ?= $(CURDIR)/../../RIOT/thirdparty_cpu
#RIOTBOARD ?= $(CURDIR)/../../RIOT/thirdparty_boards

# Uncomment this to enable scheduler statistics for ps:
#CFLAGS += -DSCHEDSTATISTICS

# If you want to use native with valgrind, you should recompile native
# with the target all-valgrind instead of all:
# make -B clean all-valgrind

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# get rid of the mandatory RPL warning
ifeq ($(shell $(CC) -Wno-cpp -E - 2>/dev/null >/dev/null dev/null ; echo $$?),0)
	ifeq ($(shell LANG=C $(CC) -Wno-cpp -E - 2>&1 1>/dev/null dev/null | grep warning: | grep -- -Wno-cpp),)
		CFLAGS += -Wno-cpp
	endif
endif

BOARD_INSUFFICIENT_RAM := chronos msb-430h redbee-econotag telosb wsn430-v1_3b wsn430-v1_4 z1

# arduino-mega2560: time.h missing from avr-libc
BOARD_BLACKLIST := arduino-mega2560

# Modules to include:

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += uart0
USEMODULE += ps
USEMODULE += vtimer
USEMODULE += msg_queue
USEMODULE += defaulttransceiver
USEMODULE += rpl
USEMODULE += udp

include $(RIOTBASE)/Makefile.include
