# name of your application
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

# If you want to use native with valgrind, you should recompile native
# with the target all-valgrind instead of all:
# make -B clean all-valgrind

# Uncomment this to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
export QUIET ?= 1

BOARD_BLACKLIST := chronos mbed_lpc1768 msb-430 msb-430h redbee-econotag \
                   telosb wsn430-v1_3b wsn430-v1_4 pttu arduino-due
# chronos: not enough RAM
# mbed_lpc1768: see https://github.com/RIOT-OS/RIOT/issues/675
# msb-430: see https://github.com/RIOT-OS/RIOT/issues/658
# msb-430h: not enough RAM
# redbee-econotag: see https://github.com/RIOT-OS/RIOT/issues/676
# telosb: not enough RAM
# wsn430-v1_3b: not enough RAM
# wsn430-v1_4: not enough RAM
# pttu: see https://github.com/RIOT-OS/RIOT/issues/659

# Modules to include:

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += posix
USEMODULE += ps
USEMODULE += random
USEMODULE += defaulttransceiver
USEMODULE += rtc
USEMODULE += crypto_sha256
USEMODULE += ccn_lite
USEMODULE += ccn_lite_client

include $(RIOTBASE)/Makefile.include
