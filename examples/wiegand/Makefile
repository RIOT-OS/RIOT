# name of your application
APPLICATION = wiegand

#DEVELHELP = 1

# If no BOARD is found in the environment, use this default:
BOARD ?= arduino-zero

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# You must include the wiegand module
USEMODULE += wiegand

include $(RIOTBASE)/Makefile.include

# Following parameters only appply if using defaults/or SAUL

# Define the D0 input pin
CFLAGS += -DWIEGAND_D0='GPIO_PIN(PA,8)'

# Define the D1 input pin
CFLAGS += -DWIEGAND_D1='GPIO_PIN(PA,7)'

# Change to falling/rising. Usefull when using
# level converters with pull-ups/pull-downs.
# By the way the protocol works normally it doesn't
# matter if you use a falling or rising edge.
CFLAGS += -DWIEGAND_FLANK=GPIO_FALLING
