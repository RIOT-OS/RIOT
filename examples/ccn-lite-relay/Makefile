# name of your application
APPLICATION = ccn-lite-relay

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

# Uncomment this to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

BOARD_INSUFFICIENT_RAM := chronos msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1 redbee-econotag
BOARD_BLACKLIST := arduino-due mbed_lpc1768 msb-430 pttu udoo qemu-i386 \
                   stm32f0discovery stm32f3discovery stm32f4discovery
# mbed_lpc1768:     see https://github.com/RIOT-OS/RIOT/issues/675
# msb-430:          see https://github.com/RIOT-OS/RIOT/issues/658
# pttu:             see https://github.com/RIOT-OS/RIOT/issues/659
# qemu-i386:        no transceiver, yet
# stm32f0discovery: no transceiver, yet
# stm32f3discovery: no transceiver, yet
# stm32f4discovery: no transceiver, yet

# Modules to include:

USEMODULE += posix

USEMODULE += defaulttransceiver
USEMODULE += rtc
USEMODULE += ccn_lite

include $(RIOTBASE)/Makefile.include
