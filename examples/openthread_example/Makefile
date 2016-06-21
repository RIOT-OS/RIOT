APPLICATION = openthread_example

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP -Wall

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEPKG=openthread

USEMODULE+=openthread_contrib
USEMODULE+=openthread_crypto
USEMODULE+=openthread_cli

USEMODULE+=xtimer
USEMODULE += gnrc_netdev_default
USEMODULE+=random

include $(RIOTBASE)/Makefile.include
