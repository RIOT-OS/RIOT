# name of your application
export APPLICATION = ccn-lite-client

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

BOARD_INSUFFICIENT_RAM := chronos msb-430h telosb wsn430-v1_3b wsn430-v1_4
BOARD_BLACKLIST := arduino-due mbed_lpc1768 msb-430 pttu redbee-econotag udoo z1 qemu-i386
# mbed_lpc1768: see https://github.com/RIOT-OS/RIOT/issues/675
# msb-430: see https://github.com/RIOT-OS/RIOT/issues/658
# pttu: see https://github.com/RIOT-OS/RIOT/issues/659
# redbee-econotag: see https://github.com/RIOT-OS/RIOT/issues/676
# z1: lacks RTC features
# qemu-i386: no tranceiver, yet

# Modules to include:

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += posix
USEMODULE += ps
USEMODULE += random
USEMODULE += defaulttransceiver
USEMODULE += rtc
USEMODULE += ccn_lite
USEMODULE += ccn_lite_client

include $(RIOTBASE)/Makefile.include
