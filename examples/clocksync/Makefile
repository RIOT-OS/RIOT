# name of your application
APPLICATION = clocksync

# If no BOARD is found in the environment, use this default:
BOARD ?= native

SYNCMOD ?= gtsp
#export SYNCMOD ?= ftsp
#export SYNCMOD ?= pulsesync

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

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

LINKFLAGS += -lm

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:

BOARD_INSUFFICIENT_RAM := chronos msb-430h redbee-econotag telosb wsn430-v1_3b wsn430-v1_4 z1
BOARD_WHITELIST := avsextrem chronos msb-430h msba2 native wsn430-v1_3b
BOARD_BLACKLIST := arduino-due mbed_lpc1768 msb-430 udoo qemu-i386 stm32f0discovery \
                   stm32f3discovery stm32f4discovery pca10000 pca10005 pttu redbee-econotag \
                   telosb wsn430-v1_4 z1 iot-lab_M3 arduino-mega2560
# mbed_lpc1768:     see https://github.com/RIOT-OS/RIOT/issues/675
# msb-430:          see https://github.com/RIOT-OS/RIOT/issues/658
# arduino-due:      no transceiver, yet
# mbed_lpc1768:     no transceiver, yet
# stm32f4discovery: no transceiver, yet
# qemu-i386:        no transceiver, yet
# stm32f0discovery: no transceiver, yet
# stm32f3discovery: no transceiver, yet
# stm32f4discovery: no transceiver, yet
# pca10000:         no transceiver, yet
# pca10005:         no transceiver, yet
# udoo:             no transceiver, yet
# stm32f0discovery: no transceiver, yet
# stm32f3discovery: no transceiver, yet
# arduino-mega2560: no transceiver, yet
# pttu:             see https://github.com/RIOT-OS/RIOT/issues/659
# wsn430-v1_4		no gtimer toa support for cc2420, yet
# wsn430-v1_3b		no gtimer toa support for cc2420, yet
# telosb			no gtimer toa support for cc2420, yet
# z1				no gtimer toa support for cc2420, yet
# redbee-econotag   no gtimer toa support for maca, yet
# iot-lab_M3   		no gtimer toa support for at86rf231, yet

USEMODULE += posix
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += defaulttransceiver
USEMODULE += random
USEMODULE += $(SYNCMOD)
USEMODULE += clocksync_eval

include $(RIOTBASE)/Makefile.include
