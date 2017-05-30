# name of your application
APPLICATION = pinoccio-rgb-thread

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 0

# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

USEMODULE += rgbled
USEMODULE += xtimer
FEATURES_REQUIRED += periph_timer
FEATURES_REQUIRED += periph_gpio
include $(RIOTBASE)/Makefile.include

