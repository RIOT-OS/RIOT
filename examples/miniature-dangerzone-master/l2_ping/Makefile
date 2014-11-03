# name of your application
APPLICATION = l2_ping

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
export RIOTBASE ?= $(CURDIR)/../../RIOT

# Change this to 0 show compiler invocation lines by default:
export QUIET ?= 1

BOARD_BLACKLIST := arduino-due mbed_lpc1768 msb-430 udoo
#arduino-due: no radio
#mbed_lpc1768: no radio
#msb-430: no radio driver
#udoo: no radio

# Modules to include:

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += uart0
USEMODULE += ps
USEMODULE += defaulttransceiver
USEMODULE += l2_ping

include $(RIOTBASE)/Makefile.include
