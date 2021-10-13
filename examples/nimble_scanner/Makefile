# name of your application
APPLICATION = nimble_scanner

# If no BOARD is found in the environment, use this default:
BOARD ?= nrf52dk

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# We use the xtimer and the shell in this example
USEMODULE += shell

# configure and use Nimble
USEPKG += nimble
USEMODULE += nimble_scanner
USEMODULE += nimble_scanlist

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
