# name of your application
export PROJECT = rpl_udp

# If no BOARD is found in the environment, use this default:
export BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
export RIOTBASE ?= $(CURDIR)/../..

# Uncomment these lines if you want to use platform support from external
# repositories:
#export RIOTCPU ?= $(CURDIR)/../../RIOT/thirdparty_cpu
#export RIOTBOARD ?= $(CURDIR)/../../RIOT/thirdparty_boards

# Uncomment this to enable scheduler statistics for ps:
#CFLAGS += -DSCHEDSTATISTICS

# If you want to use native with valgrind, you should recompile native
# with the target all-valgrind instead of all:
# make -B clean all-valgrind

# Uncomment this to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
export QUIET ?= 1

# get rid of the mandatory RPL warning
ifeq ($(shell $(CC) -Wno-cpp -E - 2>/dev/null >/dev/null dev/null ; echo $$?),0)
	ifeq ($(shell LANG=C $(CC) -Wno-cpp -E - 2>&1 1>/dev/null dev/null | grep warning: | grep -- -Wno-cpp),)
		CFLAGS += -Wno-cpp
	endif
endif

# Modules to include:

USEMODULE += auto_init
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += posix
USEMODULE += ps
USEMODULE += vtimer
ifeq ($(strip $(BOARD)),native)
	USEMODULE += nativenet
else ifeq ($(strip $(BOARD)),msba2)
	USEMODULE += cc110x
else ifeq ($(strip $(BOARD)),iot-lab_M3)
	USEMODULE += at86rf231
endif
USEMODULE += sixlowpan
USEMODULE += rpl
USEMODULE += destiny

export INCLUDES += -I$(RIOTBASE)/sys/net/include -I$(RIOTBASE)/sys/net/routing/rpl -I$(RIOTBASE)/drivers/cc110x

include $(RIOTBASE)/Makefile.include
