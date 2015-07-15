# name of your application
APPLICATION = aodvv2_tests
include ../Makefile.tests_common

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This test has not been verified to work on any other boards-- proceed with caution.
BOARD_WHITELIST := native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP
CFLAGS += -DRIOT
CFLAGS += -DFIB_DEVEL_HELPER

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include
USEMODULE += defaulttransceiver
USEMODULE += aodvv2
USEMODULE += udp

export INCLUDES += -I$(RIOTBASE)/sys/net/routing/aodvv2/

include $(RIOTBASE)/Makefile.include
