# Set the name of your application:
APPLICATION = sniffer

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# blacklist all boards without radio support
BOARD_BLACKLIST := arduino-due arduino-mega2560 mbed_lpc1768 msb-430 pttu \
				   udoo qemu-i386 stm32f0discovery stm32f3discovery \
				   stm32f4discovery pca10000 pca10005 msbiot samr21-xpro \
				   yunjia-nrf51822

# Modules to include:

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += uart0
USEMODULE += ps
USEMODULE += vtimer
USEMODULE += auto_init
USEMODULE += defaulttransceiver

include $(RIOTBASE)/Makefile.include
