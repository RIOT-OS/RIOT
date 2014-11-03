# Set the name of your application:
export PROJECT = chronos_default

# If no BOARD is found in the environment, use this default:
export BOARD ?= chronos 

# This has to be the absolute path to the RIOT base directory:
export RIOTBASE ?= $(CURDIR)/../../RIOT

# Change this to 0 show compiler invocation lines by default:
export QUIET ?= 1

# Modules to include:
USEMODULE += transceiver
USEMODULE += cc110x_ng
#board_buzzer board_display
USEMODULE += auto_init
#display_putchar battery  vti_ps


include $(RIOTBASE)/Makefile.include
