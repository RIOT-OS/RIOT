# name of your application
APPLICATION = zeno_sub

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# USEMODULE += posix_headers

USEMODULE += pthread
USEPKG += zenoh

DEVELHELP ?= 1


include $(RIOTBASE)/Makefile.include
