# name of your project
export PROJECT = ccn-lite-client

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

# If you want to use valgrind, you should recompile native with either
# HAVE_VALGRIND_H or HAVE_VALGRIND_VALGRIND_H depending on the location
# of the valgrind header (i.e. <valgrind.h> or <valgrind/valgrind.h>)
# For more information about the valgrind support of RIOT read:
# RIOT/cpu/native/README
#CFLAGS += -DHAVE_VALGRIND_VALGRIND_H
#CFLAGS += -DHAVE_VALGRIND_H

# Uncomment this to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
export QUIET ?= 1

# Modules to include:

USEMODULE += config
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += posix
USEMODULE += ps
USEMODULE += auto_init
USEMODULE += hwtimer

USEMODULE += transceiver
ifeq ($(BOARD),msba2)
	USEMODULE += cc110x_ng
else ifeq ($(BOARD),native)
	USEMODULE += nativenet
endif

USEMODULE += rtc
USEMODULE += crypto_sha256
USEMODULE += ccn_lite
USEMODULE += ccn_lite_client

export INCLUDES = -I$(RIOTBASE)/sys/net/include

include $(RIOTBASE)/Makefile.include
