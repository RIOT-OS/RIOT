# name of your application
export PROJECT = ccn-lite-relay

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

# Modules to include:

USEMODULE += config
USEMODULE += posix
USEMODULE += auto_init

USEMODULE += auto_init

USEMODULE += transceiver
ifeq ($(BOARD),msba2)
	USEMODULE += cc110x_ng
else ifeq ($(BOARD),native)
	USEMODULE += nativenet
endif

USEMODULE += rtc
USEMODULE += crypto_sha256
USEMODULE += ccn_lite

export INCLUDES = -I$(RIOTBASE)/sys/net/include/

include $(RIOTBASE)/Makefile.include
