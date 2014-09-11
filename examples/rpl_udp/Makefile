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
BOARD_BLACKLIST := arduino-due mbed_lpc1768 msb-430 pttu udoo qemu-i386 stm32f0discovery \
                   stm32f3discovery stm32f4discovery pca10000 pca10005 arduino-mega2560 \
                   msbiot
# mbed_lpc1768:     see https://github.com/RIOT-OS/RIOT/issues/675
# msb-430:          see https://github.com/RIOT-OS/RIOT/issues/658
# pttu:             see https://github.com/RIOT-OS/RIOT/issues/659
# qemu-i386:        no transceiver, yet
# stm32f0discovery: no transceiver, yet
# stm32f3discovery: no transceiver, yet
# stm32f4discovery: no transceiver, yet
# pca10000:         no transceiver, yet
# pca10005:         no transceiver, yet
# arduino-mega2560: time.h missing from avr-libc
# msbiot:           no transceiver, yet

# Modules to include:

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += vtimer
USEMODULE += defaulttransceiver
USEMODULE += rpl
USEMODULE += udp

include $(RIOTBASE)/Makefile.include
