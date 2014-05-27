# name of your application
export APPLICATION = udp-sockets

# If no BOARD is found in the environment, use this default:
export BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
export RIOTBASE ?= $(CURDIR)/../..

# Change this to 0 show compiler invocation lines by default:
export QUIET ?= 1

BOARD_INSUFFICIENT_RAM := chronos msb-430h redbee-econotag telosb wsn430-v1_3b wsn430-v1_4 z1
BOARD_BLACKLIST := arduino-due mbed_lpc1768 msb-430 pttu udoo qemu-i386
# mbed_lpc1768: see https://github.com/RIOT-OS/RIOT/issues/675
# msb-430: see https://github.com/RIOT-OS/RIOT/issues/658
# pttu: see https://github.com/RIOT-OS/RIOT/issues/659
# qemu-i386: no tranceiver, yet

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += posix
USEMODULE += pnet
USEMODULE += ps
USEMODULE += vtimer
USEMODULE += defaulttransceiver
USEMODULE += sixlowpan
USEMODULE += config
USEMODULE += destiny

include $(RIOTBASE)/Makefile.include
